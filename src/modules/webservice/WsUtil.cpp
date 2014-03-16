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

string WsUtil::generateAllWSDL(vector<string> apps,string resp,map<string,string> &wsmap,string ip_address, Reflection ref, map<string, map<string, ClassStructure> > clsstrucMaps)
{
	string ret,headers="#include \"Exception.h\"\n#include \"string\"\n#include <sstream>\n#include \"CastUtil.h\"\n#include \"XmlParser.h\"\n#include \"Reflection.h\"\n#include \"XMLSerialize.h\"\ntypedef vector<Element> ElementList;\ntypedef map<string,string> AttributeList;\n";
	for(unsigned int var = 0; var < apps.size(); ++var)
	{
		string webdir = resp+"../web/"+apps.at(var);
		ret += generateWSDL(webdir+"/config/ws.xml",clsstrucMaps[apps.at(var)],resp,headers,wsmap,apps.at(var),ip_address,ref);
	}
	return (headers + "using namespace std;\n\n" + "extern \"C\"\n{\nvoid normalizeNamespaces(Element ele, string appname, string& allnmspcs) { 	ElementList elements = ele.getChildElements(); 	string nmspc = ele.getAttribute(\"namespace\"); 	if(nmspc!=\"\") 	{ 		string nmspcid = Reflection::getNameSpaceId(nmspc, appname); 		ele.setTagName(nmspcid+\":\"+ele.getTagName()); 		ele.removeAttribute(\"namespace\"); 		allnmspcs += (\" xmlns:\" + nmspcid + \"=\\\"\" + nmspc + \"\\\"\"); 	} 	for(unsigned int i=0;i<elements.size();i++) 	{ 		normalizeNamespaces(elements.at(i), appname, allnmspcs);		} }" + ret + "\n}\n");
}

string WsUtil::generateWSDL(string file,map<string, ClassStructure> allclsmap,string resp,string &headers,map<string,string> &wsmap,string appname,string ip_address, Reflection ref)
{
	XmlParser parser("Parser");
	Document doc = parser.getDocument(file);
	Element root = doc.getRootElement();
	typedef vector<Element> ElementList;
	typedef map<string,string> strMap;
	typedef map<string,strMap> meth_Map;
	typedef map<string,meth_Map> ws_inp_out_Map;
	ElementList wsvcs = root.getChildElements();
	if(wsvcs.size()==0)
		return "";

	string ws_funcs,obj_mapng,retObj_xml;
	for(unsigned int ii=0;ii<wsvcs.size();ii++)
	{
		ws_inp_out_Map ws_info;
		meth_Map meth_info;
		StringContext gcntxt;
		string ws_name,reqr_res_bind,wsdl_msgs,wsdl_ops,wsdl_bind,wsdl,wsdl_obj_bind;
		Element ws = wsvcs.at(ii);
		ws_name = ws.getAttribute("class");
		string ows_name = ws_name;
		StringUtil::replaceAll(ws_name, "::", "_");
		if(StringUtil::trimCopy(ws_name)=="")
		{
			logger << ("No class name defined for web-service, skipping...") << endl;
			continue;
		}
		if(StringUtil::trimCopy(ws.getAttribute("location"))=="")
		{
			logger << ("No location defined for web-service, skipping...") << endl;
			continue;
		}
		gcntxt["WS_NAME"] = ws_name;
		gcntxt["WS_PATH"] = ws.getAttribute("location");
		if(gcntxt["WS_PATH"].at(0)=='/')
		{
			gcntxt["WS_PATH"] = gcntxt["WS_PATH"].substr(1);
			StringUtil::replaceAll(gcntxt["WS_PATH"], "//", "/");
		}
		gcntxt["WS_NMSPC"] = StringUtil::trimCopy(ws.getAttribute("namespace"));

		logger << ("Web service " + ws_name + " found for appname " + appname) << endl;

		ClassStructure *clstruct = NULL;
		map<string, ClassStructure>::iterator it;
		for (it=allclsmap.begin();it!=allclsmap.end();++it)
		{
			if(it->second.getFullyQualifiedClassName()==ws.getAttribute("class"))
			{
				clstruct = &it->second;
				break;
			}
		}
		if(clstruct==NULL)
		{
			logger << ("Error generating web-service artifacts, as class "+ws.getAttribute("class") + "not found...") << endl;
			continue;
		}

		if(clstruct->getNamespace()=="" && gcntxt["WS_NMSPC"]=="")
		{
			logger << ("No namespace defined for web-service, skipping...") << endl;
			continue;
		}
		if(gcntxt["WS_NMSPC"]=="")
		{
			gcntxt["WS_NMSPC"] = clstruct->getNamespace();
		}
		StringUtil::replaceAll(gcntxt["WS_NMSPC"], "::", ".");

		vector<string> pinfo;
		bool isOpForSet = false;
		strVec info = ref.getAfcObjectData(*clstruct,false,pinfo,isOpForSet);
		headers.append("#include \""+ref.getClassPath(clstruct->getTreatedClassName(true))+"\"\n");

		set<string> allnmspcs;
		map<string, string> trgnms;
		for(unsigned int i=0;i<info.size();i++)
		{
			string temp,temp11;
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

			string retType,methName,inp_params;
			if(results.size()<2)
				continue;
			if(results.size()>0 && (results.at(0)==it->second.getTreatedClassName(false) || temp.find("~")!=string::npos))
				continue;

			methName = results.at(1);

			in_out_info["RETURN"] = ws.getElementByName(methName).getAttribute("outname");
			if(in_out_info["RETURN"]=="")
				in_out_info["RETURN"] = "result";
			//logger << in_out_info["RETURN"] << flush;
			in_out_info["RETURNTYP"] = results.at(0);
			string xsdobjdef;
			if(results.at(0).find("vector<")!=string::npos)
			{
				string vecn = results.at(0);
				StringUtil::replaceFirst(vecn,"std::"," ");
				StringUtil::replaceFirst(vecn,"vector<"," ");
				StringUtil::replaceFirst(vecn,">"," ");
				StringUtil::trim(vecn);
				if(vecn=="int" || vecn=="string" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
						|| vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
				{
					if(vecn=="long long")
					{
						vecn = "long";
					}
					else if(vecn=="unsigned long long")
					{
						vecn = "unsignedlong";
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
					string trgnmspc;
					string fqcn = ref.getFullyQualifiedClassName(vecn, clstruct->getNamespaces());
					if(trgnms.find(fqcn)==trgnms.end())
					{
						xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
						trgnms[fqcn] = trgnmspc;
					}
					else
						trgnmspc = trgnms[fqcn];
					logger << "in result " + vecn + " " + trgnmspc << endl;
					if(vecn.find("::")!=string::npos)
						vecn = vecn.substr(vecn.find_last_of("::")+1);
					logger << "in result after " + vecn + " " + trgnmspc << endl;
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
							|| results.at(0)=="unsigned long")
					{
						if(results.at(0)=="long long")
						{
							results.at(0) = "long";
						}
						else if(results.at(0)=="unsigned long long")
						{
							results.at(0) = "unsignedlong";
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
						string type = results.at(0);
						string trgnmspc;
						string fqcn = ref.getFullyQualifiedClassName(type, clstruct->getNamespaces());
						if(trgnms.find(fqcn)==trgnms.end())
						{
							xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
							trgnms[fqcn] = trgnmspc;
						}
						else
							trgnmspc = trgnms[fqcn];
						logger << "in result " + type + " " + trgnmspc << endl;
						if(type.find("::")!=string::npos)
							type = type.substr(type.find_last_of("::")+1);
						logger << "in result after " + type + " " + trgnmspc << endl;
						retType = "\n<xsd:element name=\""+in_out_info["RETURN"]+"\" type=\""+trgnmspc+":"+type+"\"/>";
					}
				}
			}
			wsdl_obj_bind.append(xsdobjdef);

			for(unsigned int j=0;j<results1.size();j++)
			{
				string type;
				strVec results2;
				StringUtil::split(results2, results1.at(j), (" "));
				if(results2.size()<2)
				{
					results2.push_back("in"+CastUtil::lexical_cast<string>(j+1));
				}
				type = results2.at(0);
				int srn = j;
				stringstream ss;
				ss << srn;
				string te;
				ss >> te;
				if(type=="int" || type=="string" || type=="short" || type=="float" || type=="double" || type=="bool" || type=="long long" || type=="long"
						|| type=="unsigned int" || type=="unsigned short" || type=="unsigned long long" || type=="unsigned long")
				{
					string ttyp = type;
					if(type=="long long")
					{
						type = "long";
					}
					else if(type=="unsigned long long")
					{
						type = "unsignedlong";
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
						logger << ("Invalid thing happening " + results1.at(j)) << endl;
					}
					strMap allfs,tyfs;
					string xsdobjdef, xdstype;
					if(type.find("vector<")!=string::npos && results2.size()==2)
					{
						string vecn = type;
						StringUtil::replaceFirst(vecn,"vector<"," ");
						StringUtil::replaceFirst(vecn,"std::"," ");
						StringUtil::replaceFirst(vecn,">"," ");
						StringUtil::trim(vecn);
						if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
								|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
						{
							if(vecn=="long long")
							{
								vecn = "long";
							}
							else if(vecn=="unsigned long long")
							{
								vecn = "unsignedlong";
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
							string trgnmspc;
							string fqcn = ref.getFullyQualifiedClassName(vecn, clstruct->getNamespaces());
							type = "std::vector<" + fqcn + ">";
							if(trgnms.find(fqcn)==trgnms.end())
							{
								xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
								trgnms[fqcn] = trgnmspc;
							}
							else
								trgnmspc = trgnms[fqcn];
							logger << "in result " + vecn + " " + trgnmspc << endl;
							if(vecn.find("::")!=string::npos)
								vecn = vecn.substr(vecn.find_last_of("::")+1);
							logger << "in result after " + vecn + " " + trgnmspc << endl;
							inp_params.append("\n<xsd:element minOccurs=\"0\" maxOccurs=\"unbounded\" name=\""+results2.at(1)+"\" type=\""+trgnmspc+":"+vecn+"\"/>");
						}
					}
					else if(results2.size()==2)
					{
						string trgnmspc;
						string fqcn = ref.getFullyQualifiedClassName(type, clstruct->getNamespaces());
						if(trgnms.find(fqcn)==trgnms.end())
						{
							xsdobjdef = ref.getXSDDefinitions(allclsmap, fqcn, ref, appname, trgnmspc, allnmspcs, gcntxt["WS_NMSPC"], resp);
							trgnms[fqcn] = trgnmspc;
						}
						else
							trgnmspc = trgnms[fqcn];
						logger << "in result " + type + " " + trgnmspc << endl;
						if(type.find("::")!=string::npos)
							type = type.substr(type.find_last_of("::")+1);
						logger << "in result after " + type + " " + trgnmspc << endl;
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
		string extranmspcs;
		if(allnmspcs.size()>0)
		{
			set<string>::iterator it;
			for(it=allnmspcs.begin();it!=allnmspcs.end();++it) {
				string nmspcid = *it;
				if(nmspcid!="tns")
				{
					string nmspcidval = Reflection::getNameSpaceIdValue(nmspcid);
					if(nmspcidval!="") {
						extranmspcs.append(" xmlns:" + nmspcid + "=\""+nmspcidval+"\"");
					}
				}
			}
		}
		gcntxt["EXTR_NMSPCS"] = extranmspcs;
		gcntxt["URL"] = "http://" + ip_address + "/" + appname;
		string wspath = gcntxt["URL"] + "/" + gcntxt["WS_PATH"];
		wsmap[wspath] = ws_name;
		wsdl = TemplateEngine::evaluate(resp+"template.wsdl",gcntxt);
		AfcUtil::writeTofile(resp+"../web/"+appname+"/public/"+ws_name+".wsdl",wsdl,true);
		//ws_funcs.append(obj_mapng);
		//ws_funcs.append(retObj_xml);
		//ws_funcs.append("extern \"C\"\n{\n");
		ws_inp_out_Map::iterator iter;
		for(iter=ws_info.begin();iter!=ws_info.end();iter++)
		{
			string ws_n = iter->first;
			meth_Map meth = iter->second;
			meth_Map::iterator iter1;
			for(iter1=meth.begin();iter1!=meth.end();iter1++)
			{
				string me_n = iter1->first;
				strMap pars = iter1->second;
				string ws_nn = ws_n;
				StringUtil::replaceAll(ws_nn, "::", "_");
				ws_funcs.append("string "+appname+me_n+ws_nn+"(Element* _req)\n{\nElement ele;\n");
				ws_funcs.append("string _retStr;\n");
				ws_funcs.append("try{\n/*_req->validateNs();*/\n");
				strMap::iterator iter2;
				string args;
				unsigned int ter = 1;
				//logger << me_n << ws_n << pars.size() << endl;
				for(iter2=pars.begin();iter2!=pars.end();iter2++)
				{
					if(iter2->first!="RETURN" && iter2->first!="RETURNTYP")
					{
						string argname =  iter2->first;
						if(iter2->second=="int" || iter2->second=="short" || iter2->second=="double" || iter2->second=="float" || iter2->second=="string"
								|| iter2->second=="bool" || iter2->second=="long" || iter2->second=="long long" || iter2->second=="unsigned int"
									|| iter2->second=="unsigned short" || iter2->second=="unsigned long" || iter2->second=="unsigned long long"
											|| iter2->second=="string")
						{
							ws_funcs.append("ele = _req->getElementByName(\""+argname+"\");\n");
							ws_funcs.append(iter2->second+" "+argname+";\n");
							ws_funcs.append("if(ele.getTagName()!=\"\")");
							ws_funcs.append(argname+" = CastUtil::lexical_cast<"+iter2->second+">(ele.getText());\n");
						}
						else if(iter2->second.find("vector<")!=string::npos)
						{
							string vecn = iter2->second;
							StringUtil::replaceFirst(vecn,"std::"," ");
							StringUtil::replaceFirst(vecn,"vector<"," ");
							StringUtil::replaceFirst(vecn,">"," ");
							StringUtil::trim(vecn);
							if(vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
									|| vecn=="string" || vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
							{
								ws_funcs.append("vector<"+vecn+" > "+argname+";\n");
								ws_funcs.append("ElementList list = _req->getElementsByName(\""+argname+"\");\n");
								ws_funcs.append("for(int i=0;i<list.size();i++)");
								ws_funcs.append(argname+".push_back(CastUtil::lexical_cast<"+vecn+">(list.at(i).getText()));\n");
							}
							else
							{
								ws_funcs.append("vector<"+vecn+" > "+argname+";\n");
								ws_funcs.append("ElementList list = _req->getElementsByName(\""+argname+"\");\n");
								ws_funcs.append("for(int i=0;i<list.size();i++)\n{\n");
								ws_funcs.append(argname+".push_back(XMLSerialize::unserialize<vector<"+vecn+" > >(list.at(i).renderSerialization()));\n}\n");
							}
						}
						else
						{
							ws_funcs.append("ele = _req->getElementByName(\""+argname+"\");\n");
							ws_funcs.append(iter2->second+" "+argname+";\n");
							ws_funcs.append("if(ele.getTagName()!=\"\")");
							ws_funcs.append("{\nele.setTagName(\""+iter2->second+"\");");
							//ref.getTreatedFullyQualifiedClassName(argname, )
							ws_funcs.append("\n"+argname+" = XMLSerialize::unserialize<"+iter2->second+">(ele.renderSerialization());\n");
							ws_funcs.append("\ncout << ele.renderSerialization() << endl;");
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
						|| pars["RETURNTYP"]=="unsigned long long" || pars["RETURNTYP"]=="unsigned long")
				{
					ws_funcs.append(pars["RETURNTYP"]+" _retval;\n");
					ws_funcs.append("_retval = _obj."+me_n+"("+args+");\n");
					ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+CastUtil::lexical_cast<string>(_retval)+\"</tns:"+pars["RETURN"]+">\";\n");
				}
				else if(pars["RETURNTYP"]!="")
				{
					ws_funcs.append(pars["RETURNTYP"]+" _retval;\n");
					ws_funcs.append("_retval = _obj."+me_n+"("+args+");\n");
					if(pars["RETURNTYP"].find("vector<")!=string::npos)
					{
						string vecn = pars["RETURNTYP"];
						StringUtil::replaceFirst(vecn,"std::"," ");
						StringUtil::replaceFirst(vecn,"vector<"," ");
						StringUtil::replaceFirst(vecn,">"," ");
						StringUtil::trim(vecn);
						if(vecn=="string" || vecn=="int" || vecn=="short" || vecn=="float" || vecn=="double" || vecn=="bool" || vecn=="long long" || vecn=="long"
								|| vecn=="unsigned int" || vecn=="unsigned short" || vecn=="unsigned long long" || vecn=="unsigned long")
						{
							ws_funcs.append("for(int i=0;i<_retval.size();i++)");
							ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+CastUtil::lexical_cast<string>(_retval.at(i))+\"</tns:"+pars["RETURN"]+">\";\n");
						}
						else
						{
							ws_funcs.append("string allnmspcs;\n");
							ws_funcs.append("for(int i=0;i<_retval.size();i++)\n{\n");
							ws_funcs.append("XmlParser parser(\"Parser\");\n");
							ws_funcs.append("allnmspcs = \"\";\n");
							ws_funcs.append("Document doc = parser.getDocument(XMLSerialize::serialize<"+vecn+">(_retval.at(i)));\n");
							ws_funcs.append("\ncout << XMLSerialize::serialize<"+vecn+">(_retval.at(i)) << endl;");
							ws_funcs.append("normalizeNamespaces(doc.getRootElement(),\""+appname+"\",allnmspcs);\n");
							ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+doc.getRootElement().renderChildren()+\"</tns:"+pars["RETURN"]+">\";\n}\n");
						}
					}
					else
					{
						ws_funcs.append("string allnmspcs;\n");
						ws_funcs.append("XmlParser parser(\"Parser\");\n");
						ws_funcs.append("Document doc = parser.getDocument(XMLSerialize::serialize<"+pars["RETURNTYP"]+">(_retval));\n");
						ws_funcs.append("normalizeNamespaces(doc.getRootElement(),\""+appname+"\",allnmspcs);\n");
						ws_funcs.append("_retStr += \"<tns:"+pars["RETURN"]+">\"+doc.getRootElement().renderChildren()+\"</tns:"+pars["RETURN"]+">\";\n");
					}
				}
				ws_funcs.append("_retStr += \"</tns:\" + _req->getTagName() + \"Response>\";\n");
				ws_funcs.append("}catch(const Exception& e){\n");
				ws_funcs.append("return e.getMessage();\n} ");
				ws_funcs.append("catch(const char* e){\n");
				ws_funcs.append("return string(e);\n} ");
				ws_funcs.append("catch(...){\n");
				ws_funcs.append("return \"<soap:Fault><faultcode>soap:Server</faultcode><faultstring>Exception occurred</faultstring></soap:Fault>\";\n}\n");
				ws_funcs.append("return _retStr;\n}\n");
			}
			//ws_funcs.append("}\n");
		}
		//AfcUtil::writeTofile(rtdcfp+"WsInterface.cpp",ws_funcs,true);
	}
	return ws_funcs;
}
