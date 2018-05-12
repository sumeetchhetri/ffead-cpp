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
 * ConfigurationHandler.cpp
 *
 *  Created on: 19-Jun-2012
 *      Author: sumeetc
 */

#include "ConfigurationHandler.h"

ConfigurationHandler::ConfigurationHandler() {
	// TODO Auto-generated constructor stub

}

ConfigurationHandler::~ConfigurationHandler() {
	// TODO Auto-generated destructor stub
}

void ConfigurationHandler::normalizeUrl(const std::string& appName, std::string& url) {
	if(url=="")return;
	if(StringUtil::startsWith(url, "regex(") && StringUtil::endsWith(url, ")")) {
		std::string regurl = url.substr(6, url.length()-7);
		if(!RegexUtil::isValidRegex(regurl)) {
			url = "";
		}
		return;
	}
	if(url.at(0)!='/')
		url = "/" + url;
	if(url.find("/")==0 && url.find("/"+appName+"/")!=0) {
		url = "/" + appName + url;
	}
	RegexUtil::replace(url, "[/]+", "/");
	StringUtil::trim(url);
}

/*void ConfigurationHandler::listi(const std::string& cwd, const std::string& type, const bool& apDir, strVec &folders, const bool& showHidden)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	FILE *pipe_fp;
	std::string command;
	if(chdir(cwd.c_str())!=0)
		return;
	if(type=="/")
	{
		#ifndef OS_MINGW
		command = ("find . \\( ! -name . -prune \\) \\( -type d -o -type l \\) 2>/dev/null");
		#else
		command = ("sh -c \"find . \\( ! -name . -prune \\) \\( -type d -o -type l \\) 2>/dev/null\"");
		#endif
	}
	else
	{
		#ifndef OS_MINGW
		command = ("find . \\( ! -name . -prune \\) \\( -type f -o -type l \\) -name '*"+type+"' 2>/dev/null");
		#else
		command = ("sh -c \"find . \\( ! -name . -prune \\) \\( -type f -o -type l \\) -name '*"+type+"' 2>/dev/null\"");
		#endif
	}
	//command = ("ls -F1 "+cwd+"|grep '"+type+"'");
	logger << ("Searching directory " + cwd + " for pattern " + type) << std::endl;
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
	}
	int t_char;
	std::string folderName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			std::stringstream ss;
			ss << (char)t_char;
			std::string temp;
			ss >> temp;
			folderName.append(temp);
		}
		else if(folderName!="")
		{
			StringUtil::replaceFirst(folderName,"*","");
			StringUtil::replaceFirst(folderName,"./","");
			if(folderName.find("~")==std::string::npos && (showHidden || (!showHidden && folderName.find(".")!=0)))
			{
				if(apDir)
				{
					if(type=="/")
					{
						folderName = cwd+"/"+folderName+"/";
						RegexUtil::replace(folderName,"[/]+","/");
						folders.push_back(folderName);
					}
					else
					{
						folderName = cwd+"/"+folderName;
						RegexUtil::replace(folderName,"[/]+","/");
						folders.push_back(folderName);
					}
				}
				else
					folders.push_back(folderName);
			}
			folderName = "";
		}
	}
	pclose(pipe_fp);
}*/

void ConfigurationHandler::handle(strVec webdirs, const strVec& webdirs1, const std::string& incpath, const std::string& rtdcfpath, const std::string& serverRootDirectory, const std::string& respath)
{
	QueryClause::init();
	QueryOperator::init();
	JoinType::init();

	Reflection ref;
	//First initialize framework markers
	//Start Rest-full markers
	ref.addMarker(Marker("@RestController", Marker::TYPE_CLASS, MarkerHandler::collectStr(1, "path")));
	ref.addMarker(Marker("@GET", Marker::TYPE_METH, MarkerHandler::collectStr(4, "statusCode", "path", "icontentType", "ocontentType"),
			MarkerHandler::collectBool(4, false, true, false, false)));
	ref.addMarker(Marker("@PUT", Marker::TYPE_METH, MarkerHandler::collectStr(4, "statusCode", "path", "icontentType", "ocontentType"),
			MarkerHandler::collectBool(4, false, true, false, false)));
	ref.addMarker(Marker("@POST", Marker::TYPE_METH, MarkerHandler::collectStr(4, "statusCode", "path", "icontentType", "ocontentType"),
			MarkerHandler::collectBool(4, false, true, false, false)));
	ref.addMarker(Marker("@DELETE", Marker::TYPE_METH, MarkerHandler::collectStr(4, "statusCode", "path", "icontentType", "ocontentType"),
			MarkerHandler::collectBool(4, false, true, false, false)));
	ref.addMarker(Marker("@PathParam", Marker::TYPE_ARG, MarkerHandler::collectStr(1, "name")));
	ref.addMarker(Marker("@QueryParam", Marker::TYPE_ARG, MarkerHandler::collectStr(1, "name")));
	ref.addMarker(Marker("@FormParam", Marker::TYPE_ARG, MarkerHandler::collectStr(1, "name")));
	ref.addMarker(Marker("@HeaderParam", Marker::TYPE_ARG, MarkerHandler::collectStr(1, "name")));
	ref.addMarker(Marker("@MultipartContent", Marker::TYPE_ARG, MarkerHandler::collectStr(1, "name")));
	ref.addMarker(Marker("@Body", Marker::TYPE_ARG));
	ref.addMarker(Marker("@DefaultValue", Marker::TYPE_ARG, MarkerHandler::collectStr(1, "value")));

	//Start Web-Service markers
	ref.addMarker(Marker("@WebService", Marker::TYPE_CLASS, MarkerHandler::collectStr(2, "location", "namespc"),
			MarkerHandler::collectBool(2, true, false)));
	ref.addMarker(Marker("@WebServiceMethod", Marker::TYPE_METH, MarkerHandler::collectStr(1, "outName"),
			MarkerHandler::collectBool(1, false)));

	ref.addMarker(Marker("@JobFunction", Marker::TYPE_METH, MarkerHandler::collectStr(3, "cron", "name", "scope,singleton"),
			MarkerHandler::collectBool(3, true, true, false)));

	ref.addMarker(Marker("@Secure", Marker::TYPE_CLASS, MarkerHandler::collectStr(2, "role", "providerName")));
	ref.addMarker(Marker("@Secure", Marker::TYPE_METH, MarkerHandler::collectStr(2, "role", "providerName")));
	ref.addMarker(Marker("@SecurityProvider", Marker::TYPE_CLASS,
			MarkerHandler::collectStr(7, "providerName", "url", "welcomefile", "usernamefld", "usernamefrom", "passwordfld", "passwordfrom"),
			MarkerHandler::collectBool(7, true, true, false, true, true, true, true)));

	ref.addMarker(Marker("@Template", Marker::TYPE_CLASS, MarkerHandler::collectStr(3, "path", "file", "scope,singleton"),
			MarkerHandler::collectBool(3, true, true, false)));
	ref.addMarker(Marker("@Dview", Marker::TYPE_CLASS, MarkerHandler::collectStr(2, "path", "scope,singleton"),
			MarkerHandler::collectBool(2, true, false)));
	ref.addMarker(Marker("@Controller", Marker::TYPE_CLASS, MarkerHandler::collectStr(2, "path", "scope,singleton"),
			MarkerHandler::collectBool(2, true, false)));
	ref.addMarker(Marker("@AjaxInterface", Marker::TYPE_CLASS, MarkerHandler::collectStr(1, "path")));
	ref.addMarker(Marker("@Filter", Marker::TYPE_CLASS,
			MarkerHandler::collectStr(3, "type,,in,out,handle", "path", "scope,singleton"),
			MarkerHandler::collectBool(3, true, true, false)));
	ref.addMarker(Marker("@WebSocketHandler", Marker::TYPE_CLASS, MarkerHandler::collectStr(2, "path", "scope,singleton"),
			MarkerHandler::collectBool(2, true, false)));

	ref.addMarker(Marker("@IgnoreSer", Marker::TYPE_PROP));

	PropFileReader pread;
	propMap srprps = pread.getProperties(respath+"/server.prop");

	bool isLoggingEnabled = true;
	if(StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])!="true") {
		isLoggingEnabled = StringUtil::toLowerCopy(srprps["LOGGING_ENABLED"])=="true";
	}

	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	strVec all,afcd,appf,wspath,handoffVec;
	std::string includeRef;
	StringContext cntxt;
	std::string libs,ilibs,isrcs,iobjs,ideps,ipdobjs;
	std::vector<bool> stat;
	strVec vecvp,pathvec;
	std::map<std::string, std::string> ajintpthMap, tpes, dcps, compnts;
	propMap srp;
	XmlParser parser("Parser");
#ifdef INC_COMP
	ComponentGen gen;
#endif
	if(SSLHandler::getInstance()->getIsSSL())
	{
		propMap sslsec = pread.getProperties(respath+"/security.prop");
		if(sslsec.size()>0)
		{
			ConfigurationData::getInstance()->securityProperties.cert_file = serverRootDirectory + sslsec["CERTFILE"];
			ConfigurationData::getInstance()->securityProperties.key_file = serverRootDirectory + sslsec["KEYFILE"];
			ConfigurationData::getInstance()->securityProperties.dh_file = serverRootDirectory + sslsec["DHFILE"];
			ConfigurationData::getInstance()->securityProperties.ca_list = serverRootDirectory + sslsec["CA_LIST"];
			ConfigurationData::getInstance()->securityProperties.rand_file = serverRootDirectory + sslsec["RANDOM"];
			ConfigurationData::getInstance()->securityProperties.sec_password = sslsec["PASSWORD"];
			std::string tempcl = sslsec["CLIENT_SEC_LEVEL"];
			ConfigurationData::getInstance()->securityProperties.srv_auth_prvd = sslsec["SRV_AUTH_PRVD"];
			ConfigurationData::getInstance()->securityProperties.srv_auth_mode = sslsec["SRV_AUTH_MODE"];
			ConfigurationData::getInstance()->securityProperties.srv_auth_file = serverRootDirectory + sslsec["SRV_AUTH_FILE"];
			std::string alpnprotolist = sslsec["ALPN_PROTO_LIST"];
			if(tempcl!="")
			{
				try
				{
					ConfigurationData::getInstance()->securityProperties.client_auth = CastUtil::lexical_cast<int>(tempcl);
				}
				catch(...)
				{
					logger << "Invalid client auth level defined" << std::endl;
					ConfigurationData::getInstance()->securityProperties.client_auth = 0;
				}
			}
			ConfigurationData::getInstance()->securityProperties.isDHParams = true;
			try
			{
				ConfigurationData::getInstance()->securityProperties.isDHParams = CastUtil::lexical_cast<bool>(sslsec["ISDH_PARAMS"]);
			}
			catch(...)
			{
				logger << "Invalid boolean value for isDHParams defined" << std::endl;
			}
			ConfigurationData::getInstance()->securityProperties.alpnEnabled = true;
			try
			{
				ConfigurationData::getInstance()->securityProperties.alpnEnabled = CastUtil::lexical_cast<bool>(sslsec["ALPN_ENABLED"]);
				if(alpnprotolist!="") {
					logger << "ALPN protocols list = " << alpnprotolist << std::endl;
					ConfigurationData::getInstance()->securityProperties.alpnProtoList = StringUtil::splitAndReturn<std::vector<std::string> >(alpnprotolist, ",");
				} else {
					logger << "No ALPN protocols defined............." << std::endl;
				}
			}
			catch(...)
			{
				logger << "Invalid boolean value for alpnEnabled defined" << std::endl;
			}
		}
	}
	std::string rundyncontent;
	std::string ajrt;

	std::map<std::string, std::map<std::string, ClassStructure> > clsstrucMaps;

	for(unsigned int var=0;var<webdirs.size();var++)
	{
		//logger <<  webdirs.at(0) << std::flush;
		std::string defpath = webdirs.at(var);
		std::string dcppath = defpath + "dcp/";
		RegexUtil::replace(dcppath,"[/]+","/");
		std::string webpubpath = defpath + "public/";
		RegexUtil::replace(webpubpath,"[/]+","/");
		std::string tmplpath = defpath + "tpe/";
		RegexUtil::replace(tmplpath,"[/]+","/");
		std::string cmppath = defpath + "components/";
		RegexUtil::replace(cmppath,"[/]+","/");
		std::string usrincludes = defpath + "include/";
		RegexUtil::replace(usrincludes,"[/]+","/");
		//propMap srp = pread.getProperties(defpath+"config/app.prop");

		std::string name = webdirs1.at(var);
		StringUtil::replaceAll(name,"/","");
		rundyncontent += "cp -Rf $FFEAD_CPP_PATH/public/* $FFEAD_CPP_PATH/web/"+name+"/public/\n";
		ConfigurationData::getInstance()->servingContexts[name] = true;
		CommonUtils::addContext(name);

		std::vector<std::string> adcps;
		CommonUtils::listFiles(adcps, dcppath, ".dcp");
		//listi(dcppath,".dcp",true,adcps,false);
		for (int var = 0; var < (int)adcps.size(); ++var) {
			dcps[adcps.at(var)] = name;
		}
		std::vector<std::string> atpes;
		CommonUtils::listFiles(atpes, tmplpath, ".tpe");
		//listi(tmplpath,".tpe",true,atpes,false);
		for (int var = 0; var < (int)atpes.size(); ++var) {
			//tpes[atpes.at(var)] = name;
		}

		std::vector<std::string> acompnts;
		CommonUtils::listFiles(acompnts, cmppath, ".cmp");
		//listi(cmppath,".cmp",true,acompnts,false);
		for (int var = 0; var < (int)acompnts.size(); ++var) {
			compnts[acompnts.at(var)] = name;
		}

		all.push_back(usrincludes);
		appf.push_back(defpath+"app.xml");

		ilibs += ("-I" + usrincludes+" ");

		std::vector<std::string> includes;
		CommonUtils::listFiles(includes, usrincludes, ".h");
		//listi(usrincludes, ".h",true,includes,false);
		std::map<std::string, ClassStructure> allclsmap;
		for (unsigned int ind = 0; ind < includes.size(); ++ind)
		{
			std::map<std::string, ClassStructure> clsmap = ref.getClassStructures(includes.at(ind), name);
			std::map<std::string, ClassStructure>::iterator it;
			allclsmap.insert(clsmap.begin(), clsmap.end());
		}
		clsstrucMaps[name] = allclsmap;
		std::string tinc,ttypedefs,tclasses,tmethods,topers;
		ref.generateClassDefinition(allclsmap,tinc,ttypedefs,tclasses,tmethods,topers,name);

		std::string applibname = name;
		StringUtil::replaceAll(applibname, "-", "_");
		RegexUtil::replace(applibname, "[^a-zA-Z0-9_]+", "");

		ConfigurationData::getInstance()->securityObjectMap[name];
		ConfigurationData::getInstance()->filterObjectMap[name];
		ConfigurationData::getInstance()->controllerObjectMap[name];
		ConfigurationData::getInstance()->mappingObjectMap[name];
		ConfigurationData::getInstance()->mappingextObjectMap[name];
		ConfigurationData::getInstance()->rstCntMap[name];
		ConfigurationData::getInstance()->templateMappingMap[name];
		ConfigurationData::getInstance()->dcpMappingMap[name];
		ConfigurationData::getInstance()->viewMappingMap[name];
		ConfigurationData::getInstance()->ajaxInterfaceMap[name];
		ConfigurationData::getInstance()->fviewMappingMap[name];
		ConfigurationData::getInstance()->wsdlMap[name];
		ConfigurationData::getInstance()->websocketMappingMap[name];

		logger << "started reading application.xml " << std::endl;
		Document doc;
		parser.readDocument(defpath+"config/application.xml", doc);
		const Element& root = doc.getRootElement();
		if(root.getTagName()=="app" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			if(root.getAttribute("alias")!="" && StringUtil::trimCopy(root.getAttribute("alias"))!=name) {
				ConfigurationData::getInstance()->appAliases[StringUtil::trimCopy(root.getAttribute("alias"))] = name;
			}
			if(root.getAttribute("libname")!="" && StringUtil::trimCopy(root.getAttribute("libname"))!=name) {
				applibname = StringUtil::trimCopy(root.getAttribute("libname"));
			}
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="controllers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="controller")
						{
							std::string url = cntrls.at(cntn).getAttribute("path");
							StringUtil::trim(url);
							std::string clas = cntrls.at(cntn).getAttribute("class");
							StringUtil::trim(clas);
							std::string scope = cntrls.at(cntn).getAttribute("scope");
							if(url!="" && clas!="")
							{
								normalizeUrl(name, url);
								Bean bean("controller_"+clas,"",clas,scope,false,name);
								ConfigurationData::getInstance()->ffeadContext.addBean(bean);
								ConfigurationData::getInstance()->controllerObjectMap[name][url] = clas;
								logger << ("Adding Controller for " + url + " :: " + clas) << std::endl;
							}
							else
							{
								std::string from = cntrls.at(cntn).getAttribute("from");
								StringUtil::trim(from);
								std::string to = cntrls.at(cntn).getAttribute("to");
								StringUtil::trim(to);
								if(to.find("*")!=std::string::npos || to.find("regex(")!=std::string::npos)
								{
									logger << ("Skipping controller mapping as it contains */regex function") << std::endl;
									to = "";
								}
								if(from!="" && to!="")
								{
									normalizeUrl(name, from);
									ConfigurationData::getInstance()->mappingObjectMap[name][from] = to;
									logger << ("Adding Mapping for " + from + " :: " + to) << std::endl;
								}

								from = cntrls.at(cntn).getAttribute("fromext");
								StringUtil::trim(from);
								to = cntrls.at(cntn).getAttribute("toext");
								StringUtil::trim(to);
								if(to.find("*")!=std::string::npos || to.find("regex(")!=std::string::npos)
								{
									logger << ("Skipping controller extension mapping as it contains */regex function") << std::endl;
									to = "";
								}
								if(to.find(".")==0)
								{
									logger << ("Skipping controller provide 'to' file extension without dot(.)") << std::endl;
									to = "";
								}
								if(from.find("*")!=std::string::npos || from.find("regex(")!=std::string::npos)
								{
									logger << ("Skipping controller extension mapping as it contains */regex function") << std::endl;
									from = "";
								}
								if(from.find(".")==0)
								{
									logger << ("Skipping controller provide 'from' file extension without dot(.)") << std::endl;
									to = "";
								}
								if(from!="" && to!="")
								{
									ConfigurationData::getInstance()->mappingextObjectMap[name][from] = to;
									logger << ("Adding Extension Mapping for " + from + " :: " + to) << std::endl;
								}
							}
						}
					}
				}
				/*else if(eles.at(apps).getTagName()=="authhandlers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="authhandler")
						{
							std::string url = cntrls.at(cntn).getAttribute("url");
							StringUtil::trim(url);
							std::string provider = cntrls.at(cntn).getAttribute("provider");
							StringUtil::trim(provider);
							std::string scope = cntrls.at(cntn).getAttribute("scope");
							StringUtil::trim(scope);
							if(url.find("*")!=std::string::npos || url.find("regex(")!=std::string::npos)
							{
								logger << ("Skipping authhandler as login url contains *regex function") << std::endl;
								url = "";
							}
							if(url!="" && provider!="")
							{
								ConfigurationData::getInstance()->authHandlerObjectMap[name][url] = provider;
								if(provider!="" && provider.find("class:")!=std::string::npos)
								{
									std::string clas = provider.substr(provider.find(":")+1);
									Bean bean("authhandler_"+name+clas,"",clas,scope,false,name);
									ConfigurationData::getInstance()->ffeadContext.addBean(bean);
								}
								logger << ("Adding Authhandler for " + (name + url) + " :: " + provider) << std::endl;
							}
						}
					}
				}*/
				else if(eles.at(apps).getTagName()=="filters")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="filter")
						{
							std::string url = cntrls.at(cntn).getAttribute("path");
							StringUtil::trim(url);
							std::string clas = cntrls.at(cntn).getAttribute("class");
							StringUtil::trim(clas);
							std::string type = cntrls.at(cntn).getAttribute("type");
							StringUtil::trim(type);
							std::string scope = cntrls.at(cntn).getAttribute("scope");
							StringUtil::trim(scope);
							if(clas!="" && (type=="in" || type=="out" || type=="handle"))
							{
								normalizeUrl(name, url);
								if(url=="")url="*.*";
								ConfigurationData::getInstance()->filterObjectMap[name][url+type].push_back(clas);
								if(clas!="")
								{
									Bean bean("filter_"+clas,"",clas,scope,false,name);
									ConfigurationData::getInstance()->ffeadContext.addBean(bean);
								}
								logger << ("Adding Filter for " + (name + url + type) + " :: " + clas) << std::endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="templates")
				{
					ElementList tmplts = eles.at(apps).getChildElements();
					for (unsigned int tmpn = 0; tmpn < tmplts.size(); tmpn++)
					{
						if(tmplts.at(tmpn).getTagName()=="template")
						{
							std::string clas = StringUtil::trimCopy(tmplts.at(tmpn).getAttribute("class"));
							std::string url = StringUtil::trimCopy(tmplts.at(tmpn).getAttribute("path"));
							std::string file = StringUtil::trimCopy(tmplts.at(tmpn).getAttribute("file"));
							if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
							{
								normalizeUrl(name, url);
								std::string fpath = tmplpath+file;
								RegexUtil::replace(fpath,"[/]+","/");
								std::string wbpi = fpath;
								StringUtil::replaceFirst(wbpi, ConfigurationData::getInstance()->coreServerProperties.webPath, "");
								RegexUtil::replace(wbpi,"[^a-zA-Z0-9_]+","");
								ConfigurationData::getInstance()->templateMappingMap[name][url] = clas+";"+wbpi;
								tpes[fpath] = name;
								//tpes.push_back(defpath+tmplts.at(tmpn).getAttribute("file"));
								std::string scope = tmplts.at(tmpn).getAttribute("scope");
								if(clas!="")
								{
									Bean bean("template_"+clas,"",clas,scope,false,name);
									ConfigurationData::getInstance()->ffeadContext.addBean(bean);
								}
								logger << ("Adding Template for " + (url) + " :: " + clas) << std::endl;
							}
							else
							{
								logger << ("Skipping Template as path contains */regex function") << std::endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="dynamic-cpp-pages")
				{
					ElementList dycpppgs = eles.at(apps).getChildElements();
					for (unsigned int tmpn = 0; tmpn < dycpppgs.size(); tmpn++)
					{
						if(dycpppgs.at(tmpn).getTagName()=="dynamic-cpp-page")
						{
							std::string url = StringUtil::trimCopy(dycpppgs.at(tmpn).getAttribute("path"));
							std::string file = StringUtil::trimCopy(dycpppgs.at(tmpn).getAttribute("file"));
							if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
							{
								normalizeUrl(name, url);
								std::string fpath = dcppath+file;
								RegexUtil::replace(fpath,"[/]+","/");
								if(dcps.find(fpath)!=dcps.end())
								{
									std::string wbpi = fpath;
									StringUtil::replaceFirst(wbpi, ConfigurationData::getInstance()->coreServerProperties.webPath, "");
									RegexUtil::replace(wbpi,"[^a-zA-Z0-9_]+","");
									ConfigurationData::getInstance()->dcpMappingMap[name][url] = wbpi;
									logger << ("Adding Dynamic C++ Page for " + (url) + " :: " + fpath) << std::endl;
								}
								else
								{
									logger << ("Could not find Dynamic C++ Page for " + (url) + " :: " + fpath) << std::endl;
								}
							}
							else
							{
								logger << ("Skipping Template as path contains */regex function") << std::endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="web-socket-handlers")
				{
					ElementList tmplts = eles.at(apps).getChildElements();
					for (unsigned int tmpn = 0; tmpn < tmplts.size(); tmpn++)
					{
						if(tmplts.at(tmpn).getTagName()=="web-socket-handler")
						{
							std::string clas = tmplts.at(tmpn).getAttribute("class");
							std::string url = StringUtil::trimCopy(tmplts.at(tmpn).getAttribute("path"));
							if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
							{
								normalizeUrl(name, url);
								ConfigurationData::getInstance()->websocketMappingMap[name][url] = clas;
								std::string scope = tmplts.at(tmpn).getAttribute("scope");
								if(clas!="")
								{
									Bean bean("websocketclass_"+clas,"",clas,scope,false,name);
									ConfigurationData::getInstance()->ffeadContext.addBean(bean);
								}
								logger << ("Adding WebSocketHandler for " + (url) + " :: " + clas) << std::endl;
							}
							else
							{
								logger << ("Skipping WebSocketHandler as path contains */regex function") << std::endl;
							}
						}
					}
				}
#ifdef INC_DVIEW
				else if(eles.at(apps).getTagName()=="dviews")
				{
					ElementList dvs = eles.at(apps).getChildElements();
					for (unsigned int dn = 0; dn < dvs.size(); dn++)
					{
						if(dvs.at(dn).getTagName()=="dview")
						{
							std::string url = dvs.at(dn).getAttribute("path");
							normalizeUrl(name, url);
							std::string clas = dvs.at(dn).getAttribute("class");
							ConfigurationData::getInstance()->viewMappingMap[name][url] = clas;
							std::string scope = dvs.at(dn).getAttribute("scope");
							if(clas!="")
							{
								Bean bean("dview_"+clas,"",clas,scope,false,name);
								ConfigurationData::getInstance()->ffeadContext.addBean(bean);
							}
							logger << ("Adding Dynamic View for " + (name+url) + " :: " + clas) << std::endl;
						}
					}
				}
#endif
				else if(eles.at(apps).getTagName()=="ajax-interfaces")
				{
					ElementList ajintfs = eles.at(apps).getChildElements();
					for (unsigned int dn = 0; dn < ajintfs.size(); dn++)
					{
						if(ajintfs.at(dn).getTagName()=="ajax-interface")
						{
							std::string url = ajintfs.at(dn).getAttribute("path");
							if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
							{
								normalizeUrl(name, url);
								std::string clas = ajintfs.at(dn).getAttribute("class");
								ConfigurationData::getInstance()->ajaxInterfaceMap[name][url] = clas;
								pathvec.push_back(name);
								vecvp.push_back(usrincludes);
								stat.push_back(false);
								ajintpthMap[clas] = url;
								afcd.push_back(clas);
								logger << ("Adding Ajax Interface for " + (name+url) + " :: " + clas) << std::endl;
							}
							else
							{
								logger << ("Skipping Ajax Interface as path contains */regex function") << std::endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="restcontrollers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="restcontroller")
						{
							std::string url = cntrls.at(cntn).getAttribute("path");
							std::string clas = cntrls.at(cntn).getAttribute("class");
							std::string rname = "";//cntrls.at(cntn).getAttribute("name");
							std::string scope = cntrls.at(cntn).getAttribute("scope");
							ElementList resfuncs = cntrls.at(cntn).getChildElements();
							for (unsigned int cntn1 = 0; cntn1 < resfuncs.size(); cntn1++)
							{
								if(resfuncs.at(cntn1).getTagName()=="restfunction")
								{
									RestFunction restfunction;
									restfunction.name = resfuncs.at(cntn1).getAttribute("name");
									restfunction.path = resfuncs.at(cntn1).getAttribute("path");
									restfunction.clas = clas;
									restfunction.meth = StringUtil::toUpperCopy(resfuncs.at(cntn1).getAttribute("meth"));
									restfunction.statusCode = resfuncs.at(cntn1).getAttribute("statusCode");
									if(restfunction.statusCode!="")
									{
										try {
											CastUtil::lexical_cast<int>(restfunction.statusCode);
										} catch(...) {
											logger << "Rest: invalid response statusCode specified, defaulting to 200.." << std::endl;
											restfunction.statusCode = "200";
										}
									}
									/*restfunction.baseUrl = resfuncs.at(cntn1).getAttribute("baseUrl");
									if(restfunction.baseUrl!="")
									{
										if(restfunction.baseUrl.at(0)!='/')
											restfunction.baseUrl = "/" + restfunction.baseUrl;
									}*/
									restfunction.icontentType = resfuncs.at(cntn1).getAttribute("icontentType");
									restfunction.ocontentType = resfuncs.at(cntn1).getAttribute("ocontentType");
									ElementList resfuncparams = resfuncs.at(cntn1).getChildElements();
									bool hasBodyParam = false, invalidParam = false;;
									for (unsigned int cntn2 = 0; cntn2 < resfuncparams.size(); cntn2++)
									{
										if(resfuncparams.at(cntn2).getTagName()=="param")
										{
											RestFunctionParams param;
											param.type = resfuncparams.at(cntn2).getAttribute("type");
											param.from = resfuncparams.at(cntn2).getAttribute("from");
											param.name = resfuncparams.at(cntn2).getAttribute("name");
											param.defValue = resfuncparams.at(cntn2).getAttribute("defValue");
											if(param.from=="body")
											{
												hasBodyParam = true;
											}
											if(StringUtil::trimCopy(param.type)=="")
											{
												invalidParam = true;
												logger << "Rest: no type specified for param" << std::endl;
											}
											else if((param.type=="filestream" || param.type=="vector-of-filestream") && param.from!="multipart-content")
											{

											}
											else if(param.type=="vector-of-filestream" && param.from=="multipart-content"
													&& StringUtil::trimCopy(param.name)=="")
											{

											}
											else if(param.from!="body" && StringUtil::trimCopy(param.name)=="")
											{
												invalidParam = true;
												logger << "Rest: no name specified for param" << std::endl;
											}
											else if(param.from=="body" && (restfunction.meth=="GET" || restfunction.meth=="OPTIONS" || restfunction.meth=="TRACE"
													|| restfunction.meth=="HEAD"))
											{
												invalidParam = true;
												logger << ("Rest: skipping param " + param.type + ", from is body and method is " + restfunction.meth) << std::endl;
											}
											else if(!(param.type=="int" || param.type=="short" || param.type=="long" || param.type=="float" || param.type=="string"
													|| param.type=="long long" || param.type=="std::string" || param.type=="double" || param.type=="bool"
													|| param.type=="filestream" || param.type=="vector-of-filestream") && param.from!="body")
											{
												invalidParam = true;
												logger << ("Rest: skipping param " + param.type + ", from is not body and input is a complex type") << std::endl;
											}
											else if(param.from=="postparam" && (restfunction.meth=="GET" || restfunction.meth=="OPTIONS" || restfunction.meth=="TRACE"
													|| restfunction.meth=="HEAD"))
											{
												invalidParam = true;
												logger << ("Rest: skipping param " + param.type + ", from is postparam and method is " + restfunction.meth) << std::endl;
											}
											if(!invalidParam)
											{
												restfunction.params.push_back(param);
											}
										}
									}
									if(hasBodyParam && restfunction.params.size()>1)
									{
										logger << ("Rest: skipping method " + restfunction.meth + " as only one argument allowed of type body") << std::endl;
										continue;
									}
									if(invalidParam)
									{
										continue;
									}
									if(clas!="")
									{
										if(url.find("*")==std::string::npos)
										{
											if(url=="")
												url = "/" + clas;
											else if(url.at(0)!='/')
												url = "/" + url;
											std::string urlmpp;
											if(restfunction.path!="")
											{
												urlmpp = "/"+name+url+"/"+restfunction.path;
												RegexUtil::replace(urlmpp,"[/]+","/");
												restfunction.path = urlmpp;
												ConfigurationData::getInstance()->rstCntMap[name][restfunction.meth+urlmpp].push_back(restfunction);
											}
											else
											{
												urlmpp = "/"+name+url+"/"+restfunction.name;
												RegexUtil::replace(urlmpp,"[/]+","/");
												restfunction.path = urlmpp;
												ConfigurationData::getInstance()->rstCntMap[name][restfunction.meth+urlmpp].push_back(restfunction);
											}
											if(clas!="")
											{
												Bean bean("restcontroller_"+clas,"",clas,scope,false,name);
												ConfigurationData::getInstance()->ffeadContext.addBean(bean);
											}
											logger << ("Rest: Adding rest-controller => " + urlmpp  + " , class => " + clas) << std::endl;
										}
									}
								}
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="security")
				{
					ElementList allprovs = eles.at(apps).getChildElements();
					for (unsigned int cntpra = 0; cntpra < allprovs.size(); cntpra++)
					{
						if(allprovs.at(cntpra).getTagName()=="providers")
						{
							ElementList provs = allprovs.at(cntpra).getChildElements();
							for (unsigned int cntpr = 0; cntpr < provs.size(); cntpr++)
							{
								if(provs.at(cntpr).getTagName()!="provider")
									continue;

								std::string provName = provs.at(cntpr).getAttribute("name");
								if(provName=="")
								{
									logger << ("No name provided for security-provider, skipping...") << std::endl;
									continue;
								}

								if(ConfigurationData::getInstance()->securityObjectMap.find(name)!=ConfigurationData::getInstance()->securityObjectMap.end()
										&& ConfigurationData::getInstance()->securityObjectMap[name].find(provName)!=ConfigurationData::getInstance()->securityObjectMap[name].end())
								{
									logger << ("Security-provider already exists, skipping duplicate...") << std::endl;
									continue;
								}

								ElementList cntrls = provs.at(cntpr).getChildElements();
								for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
								{
									if(cntrls.at(cntn).getTagName()=="login-handler")
									{
										std::string provider = cntrls.at(cntn).getAttribute("provider");
										std::string url = cntrls.at(cntn).getAttribute("path");
										std::string sessionTimeoutV = cntrls.at(cntn).getAttribute("sessionTimeout");
										std::string scope = cntrls.at(cntn).getAttribute("scope");

										Security securityObject = ConfigurationData::getInstance()->securityObjectMap[name][provName];
										securityObject.loginProvider = provider;
										normalizeUrl(name, url);
										securityObject.loginUrl = url;
										try {
											securityObject.sessTimeout = CastUtil::lexical_cast<long>(sessionTimeoutV);
										} catch (...) {
											securityObject.sessTimeout = 3600;
											logger << ("Security: Invalid session timeout value defined, defaulting to 1hour/3600sec") << std::endl;
										}
										ConfigurationData::getInstance()->securityObjectMap[name][provName] = securityObject;
										if(provider!="" && provider.find("class:")!=std::string::npos)
										{
											std::string clas = provider.substr(provider.find(":")+1);
											Bean bean("login-handler_"+clas,"",clas,scope,false,name);
											ConfigurationData::getInstance()->ffeadContext.addBean(bean);
										}
										logger << ("Security: Adding Login Handler => " + url  + " , provider => " + provider) << std::endl;
									}
									else if(cntrls.at(cntn).getTagName()=="secure")
									{
										Security securityObject = ConfigurationData::getInstance()->securityObjectMap[name][provName];
										std::string path = cntrls.at(cntn).getAttribute("path");
										std::string role = cntrls.at(cntn).getAttribute("role");
										SecureAspect secureAspect;
										normalizeUrl(name, path);
										secureAspect.path = path;
										secureAspect.role = role;
										if(securityObject.addAspect(secureAspect))
										{
											ConfigurationData::getInstance()->securityObjectMap[name][provName] = securityObject;
											logger << ("Security: Adding Secure Path => " + path  + " , role => " + role) << std::endl;
										}
									}
									else if(cntrls.at(cntn).getTagName()=="welcome")
									{
										std::string welcomeFile = cntrls.at(cntn).getAttribute("file");
										Security securityObject = ConfigurationData::getInstance()->securityObjectMap[name][provName];
										securityObject.welcomeFile = welcomeFile;
										ConfigurationData::getInstance()->securityObjectMap[name][provName] = securityObject;
										logger << ("Security: Adding Welcome file => " + welcomeFile) << std::endl;
									}
									else if(cntrls.at(cntn).getTagName()=="username")
									{
										std::string ffrom = cntrls.at(cntn).getAttribute("from");
										std::string fname = cntrls.at(cntn).getAttribute("name");
										if(ffrom!="reqparam" && ffrom!="postparam" && ffrom!="header" && fname!="")
										{
											Security securityObject = ConfigurationData::getInstance()->securityObjectMap[name][provName];
											securityObject.securityFieldNames["username"] = fname;
											securityObject.securityFieldFrom["username"] = ffrom;
											ConfigurationData::getInstance()->securityObjectMap[name][provName] = securityObject;
										}
									}
									else if(cntrls.at(cntn).getTagName()=="password")
									{
										std::string ffrom = cntrls.at(cntn).getAttribute("from");
										std::string fname = cntrls.at(cntn).getAttribute("name");
										if(ffrom!="reqparam" && ffrom!="postparam" && ffrom!="header" && fname!="")
										{
											Security securityObject = ConfigurationData::getInstance()->securityObjectMap[name][provName];
											securityObject.securityFieldNames["password"] = fname;
											securityObject.securityFieldFrom["password"] = ffrom;
											ConfigurationData::getInstance()->securityObjectMap[name][provName] = securityObject;
										}
									}
								}
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="handoffs")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="handoff")
						{
							std::string app = cntrls.at(cntn).getAttribute("app");
							std::string def = cntrls.at(cntn).getAttribute("default");
							std::string ext = cntrls.at(cntn).getAttribute("ext");
							ConfigurationData::getInstance()->handoffs[app] = def;
							ConfigurationData::getInstance()->handoffs[app+"extension"] = ext;
							handoffVec.push_back("-l"+ app+" ");
							logger << ("Adding Handoff for app => " + app  + " , ext => " + ext  + " , default url => " + def) << std::endl;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="cors-config")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					std::string allwdOrigins, allwdMethods, allwdHeaders, exposedHeaders;
					bool allwdCredentials;
					long maxAge;
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="allow-origins")
						{
							allwdOrigins = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="allow-methods")
						{
							allwdMethods = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="allow-headers")
						{
							allwdHeaders = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="allow-credentials")
						{
							try {
								allwdCredentials = CastUtil::lexical_cast<bool>(cntrls.at(cntn).getText());
							} catch(...) {}
						}
						else if(cntrls.at(cntn).getTagName()=="expose-headers")
						{
							exposedHeaders = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="max-age")
						{
							try {
								maxAge = CastUtil::lexical_cast<long>(cntrls.at(cntn).getText());
							} catch(...) {}
						}
					}
					ConfigurationData::getInstance()->corsConfig = CorsConfig(allwdOrigins, allwdMethods, allwdHeaders,
							exposedHeaders, allwdCredentials, maxAge);
					logger << ("CORS Configuartion allow-origins => " + ConfigurationData::getInstance()->corsConfig.allwdOrigins
							+ " , allow-methods => " + ConfigurationData::getInstance()->corsConfig.allwdMethods
							+ " , allow-headers => " + ConfigurationData::getInstance()->corsConfig.allwdHeaders
							+ " , allow-credentials => " + CastUtil::lexical_cast<std::string>(ConfigurationData::getInstance()->corsConfig.allwdCredentials)
							+ " , expose-headers => " + ConfigurationData::getInstance()->corsConfig.exposedHeaders
							+ " , max-age => " + CastUtil::lexical_cast<std::string>(ConfigurationData::getInstance()->corsConfig.maxAge)) << std::endl;
				}
#ifdef INC_JOBS
				else if(eles.at(apps).getTagName()=="job-procs")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					JobScheduler::init(cntrls, name);
				}
#endif
			}
		}
		logger << "done reading application.xml " << std::endl;

		libs += ("-l"+ applibname+" ");

		configureDataSources(name, defpath+"config/sdorm.xml", allclsmap);
		configureCaches(name, defpath+"config/cache.xml");

		logger << "started reading fviews.xml " << std::endl;
		Document doc1;
		parser.readDocument(defpath+"config/fviews.xml", doc1);
		const Element& root1 = doc1.getRootElement();
		if(root1.getTagName()=="fview" && root1.getChildElements().size()>0)
		{
			ElementList eles = root1.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="page")
				{
					std::string fvw = eles.at(apps).getAttribute("htm");
					StringUtil::replaceFirst(fvw,".html",".fview");
					ConfigurationData::getInstance()->fviewMappingMap[name][fvw] = eles.at(apps).getAttribute("class");
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(false);
					normalizeUrl(name, fvw);
					ConfigurationData::getInstance()->ajaxInterfaceMap[name][fvw] = eles.at(apps).getAttribute("class");
					afcd.push_back(eles.at(apps).getAttribute("class"));
					ElementList elese = eles.at(apps).getChildElements();
					std::string nsfns = "\nvar _fview_namespace = {";
					std::string js = "\n\nwindow.onload = function(){";
					for (int appse = 0; appse < (int)elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="event")
						{
							nsfns += "\n\"_fview_cntxt_global_js_callback"+CastUtil::lexical_cast<std::string>(appse)+"\" : function(response){" + elese.at(appse).getAttribute("cb") + "},";
							js += "\ndocument.getElementById('"+elese.at(appse).getAttribute("eid")+"').";
							js += elese.at(appse).getAttribute("type") + " = function(){";
							js += eles.at(apps).getAttribute("class")+"."+elese.at(appse).getAttribute("func")+"(";
							std::string args = elese.at(appse).getAttribute("args");
							if(args!="")
								args += ",";
							js += args + "\"_fview_cntxt_global_js_callback"+CastUtil::lexical_cast<std::string>(appse)+"\",\"/"+name+"/"+fvw+"\",_fview_namespace);}";
						}
						else if(elese.at(appse).getTagName()=="form")
						{
							//ConfigurationData::getInstance()->fviewFormMap[name][elese.at(appse).getAttribute("name")] = *(elese.at(appse));
							std::string clas = elese.at(appse).getAttribute("controller");
							std::string scope = elese.at(appse).getAttribute("scope");
							if(clas!="")
							{
								Bean bean("form_"+clas,"",clas,scope,false,name);
								ConfigurationData::getInstance()->ffeadContext.addBean(bean);
							}
							elese.at(appse).copy(&(ConfigurationData::getInstance()->fviewFormMap[name][elese.at(appse).getAttribute("name")]));
							logger << ("Fview: Adding form => " + elese.at(appse).getAttribute("name")
								+ " , form class => " + elese.at(appse).getAttribute("bean")
								+ " , form controller => " + elese.at(appse).getAttribute("controller")) << std::endl;
						}
					}
					js += "\n}\n\n";
					nsfns = nsfns.substr(0,nsfns.length()-1) + "\n}\n";
					js = nsfns + js;
					for (unsigned int appse = 0; appse < elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="functions")
						{
							js += elese.at(appse).getText();
						}
					}
					AfcUtil::writeTofile(webpubpath+eles.at(apps).getAttribute("class")+".js",js,true);

					logger << ("Fview: Adding fview page class => " + eles.at(apps).getAttribute("class")
							+ " , html => " + eles.at(apps).getAttribute("htm")) << std::endl;
				}
			}
		}
		logger << "done reading fviews.xml " << std::endl;

		std::string infjs;
		ajrt += AfcUtil::generateJsInterfacessAll(allclsmap,infjs,ajintpthMap,afcd,ref);
		std::string objs = AfcUtil::generateJsObjectsAll(allclsmap);
		vecvp.clear();
		afcd.clear();
		pathvec.clear();
		ajintpthMap.clear();

		AfcUtil::writeTofile(webpubpath+"_afc_Objects.js",objs,true);
		AfcUtil::writeTofile(webpubpath+"_afc_Interfaces.js",infjs,true);

		LoggerFactory::init(defpath+"/config/logging.xml", defpath, name, isLoggingEnabled);
	}

	ConfigurationData::getInstance()->classStructureMap = clsstrucMaps;

	std::string confsrcFiles = "", confsrcFilesDinter = "";
#ifdef INC_COMP
	logger << "started generating component code" <<std::endl;
	std::map<std::string, std::string>::iterator cmpit;
	for (cmpit=compnts.begin();cmpit!=compnts.end();++cmpit)
	{
		std::string cudata,cuheader,curemote,curemoteheaders;
		try
		{
			std::string file = gen.generateComponentCU(cmpit->first,cudata,cuheader,curemote,curemoteheaders,cmpit->second);
			AfcUtil::writeTofile(rtdcfpath+file+".h",cuheader,true);
			AfcUtil::writeTofile(rtdcfpath+file+".cpp",cudata,true);
			AfcUtil::writeTofile(rtdcfpath+file+"_Remote.h",curemoteheaders,true);
			AfcUtil::writeTofile(rtdcfpath+file+"_Remote.cpp",curemote,true);
			confsrcFiles = "../" + file+".cpp ";
			confsrcFiles += "../" + file+"_Remote.cpp ";
			isrcs += "./"+file+".cpp \\\n"+"./"+file+"_Remote.cpp \\\n";
			iobjs += "./"+file+".o \\\n"+"./"+file+"_Remote.o \\\n";
			ideps += "./"+file+".d \\\n"+"./"+file+"_Remote.d \\\n";
			ConfigurationData::getInstance()->componentNames.push_back(file);
		}
		catch(const char* ex)
		{
			logger << ("Exception occurred during component code generation : ") << ex << std::endl;
		}
	}
	logger << "done generating component code" <<std::endl;
#endif
	for (unsigned int cntn = 0; cntn < handoffVec.size(); cntn++)
	{
		StringUtil::replaceFirst(libs, handoffVec.at(cntn), "");
	}
	logger << "started generating reflection/serialization code" <<std::endl;
	std::string ret = ref.generateClassDefinitionsAll(clsstrucMaps,includeRef,webdirs1);
	std::string objs, ajaxret, headers,typerefs;
	AfcUtil::writeTofile(rtdcfpath+"ReflectorInterface.cpp",ret,true);
	confsrcFiles += "../ReflectorInterface.cpp ";
	ret = ref.generateSerDefinitionAll(clsstrucMaps,includeRef, true, objs, ajaxret, headers,typerefs,webdirs1);
	AfcUtil::writeTofile(rtdcfpath+"SerializeInterface.cpp",ret,true);
	confsrcFiles += "../SerializeInterface.cpp ";
	logger << "done generating reflection/serialization code" <<std::endl;
	cntxt["RUNTIME_LIBRARIES"] = libs;
	//ret = TemplateEngine::evaluate(rtdcfpath+"objects.mk.template",cntxt);
	//AfcUtil::writeTofile(rtdcfpath+"objects.mk",ret,true);
	cntxt.clear();
	cntxt["USER_DEFINED_INC"] = ilibs;
	//cntxt["RUNTIME_COMP_SRCS"] = isrcs;
	//cntxt["RUNTIME_COMP_OBJS"] = iobjs;
	//cntxt["RUNTIME_COMP_DEPS"] = ideps;
	ipdobjs = "";

	//Process all markers
	std::vector<WsDetails> allwsdets;
	handleMarkerConfigurations(clsstrucMaps, allwsdets, stat, vecvp, pathvec, ajintpthMap, tpes, serverRootDirectory, afcd, ajrt, ref);

#ifdef INC_DCP
	ConfigurationData::getInstance()->dynamicCppPagesMap = dcps;
	logger << "started generating dcp code" <<std::endl;
	ret = DCPGenerator::generateDCPAll();
	AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
	confsrcFilesDinter = "../DCPInterface.cpp ";
	logger << "done generating dcp code" <<std::endl;
	ipdobjs += "./DCPInterface.o \\\n";
#endif
	ConfigurationData::getInstance()->templateFilesMap = tpes;
#ifdef INC_TPE
	logger << "started generating template code" <<std::endl;
	ret = TemplateGenerator::generateTempCdAll(serverRootDirectory);
	//logger << ret << std::endl;
	AfcUtil::writeTofile(rtdcfpath+"TemplateInterface.cpp",ret,true);
	confsrcFilesDinter += "../TemplateInterface.cpp ";
	logger << "done generating template code" <<std::endl;
	ipdobjs += "./TemplateInterface.o \\\n";
#endif
	std::string infjs;
	logger << std::endl<< "started generating ajax code" <<std::endl;
	std::string ajaxHeaders;
	//ret = AfcUtil::generateJsObjectsAll(vecvp,afcd,infjs,pathvec,ajintpthMap);
	ret = ajaxret + ajrt + "\n}\n";
	AfcUtil::writeTofile(rtdcfpath+"AjaxInterface.cpp",ret,true);
	confsrcFiles += "../AjaxInterface.cpp ";
	//AfcUtil::writeTofile(pubpath+"_afc_Objects.js",objs,true);
	//AfcUtil::writeTofile(pubpath+"_afc_Interfaces.js",infjs,true);
	AfcUtil::writeTofile(incpath+"AfcInclude.h",(ajaxHeaders+headers),true);
	logger << "done generating ajax code" <<std::endl;
	webdirs.clear();
#ifdef INC_APPFLOW
	ApplicationUtil apputil;
	logger << "started generating application code" <<std::endl;
	ret = apputil.buildAllApplications(appf,webdirs1);
	AfcUtil::writeTofile(rtdcfpath+"ApplicationInterface.cpp",ret,true);
	confsrcFiles += "../ApplicationInterface.cpp ";
	logger <<  "done generating application code" <<std::endl;
	iobjs += "./ApplicationInterface.o \\\n";
#endif
#ifdef INC_WEBSVC
	WsUtil wsu;
	logger <<  "started generating web-service code" <<std::endl;
	std::vector<WsDetails> allwsdets1 = wsu.getWsDetails(webdirs1, serverRootDirectory);
	std::copy(allwsdets1.begin(), allwsdets1.end(), back_inserter(allwsdets));
	ret = wsu.generateAllWSDL(allwsdets, respath, ref, clsstrucMaps);
	AfcUtil::writeTofile(rtdcfpath+"WsInterface.cpp",ret,true);
	confsrcFiles += "../WsInterface.cpp ";
	logger <<  "done generating web-service code" <<std::endl;
	iobjs += "./WsInterface.o \\\n";
#endif

	cntxt["RUNTIME_COMP_POBJS"] = iobjs;
	cntxt["RUNTIME_COMP_PDOBJS"] = ipdobjs;

	//ret = TemplateEngine::evaluate(rtdcfpath+"subdir.mk.template",cntxt);
	//AfcUtil::writeTofile(rtdcfpath+"subdir.mk",ret,true);

	cntxt.clear();
	/*if(ipdobjs=="")
		cntxt["TARGET_LIB"] = "libinter";
	else
		cntxt["TARGET_LIB"] = "all";
	cntxt["Dynamic_Public_Folder_Copy"] = rundyncontent;
	std::string cont = TemplateEngine::evaluate(respath+"/rundyn_template.sh", cntxt);
	AfcUtil::writeTofile(respath+"/rundyn.sh", cont, true);
	cntxt.clear();
	cntxt["TARGET_LIB"] = "libdinter";
	cntxt["Dynamic_Public_Folder_Copy"] = rundyncontent;
	cont = TemplateEngine::evaluate(respath+"/rundyn_template.sh", cntxt);
	AfcUtil::writeTofile(respath+"/rundyn_dinter.sh", cont, true);*/
//#if BUILT_WITH_CONFGURE == 1
	cntxt.clear();
	cntxt["INTER_DINTER_LIBRARIES"] = libs;
	cntxt["INTER_DINTER_INCLUDES"] = ilibs;
	cntxt["INTER_SOURCES"] = confsrcFiles;
	cntxt["DINTER_SOURCES"] = confsrcFilesDinter;

	std::string mkfileloc = rtdcfpath+"/autotools/Makefile.am.template";
	std::string mkfileamloc = rtdcfpath+"/autotools/Makefile.am";
	ret = TemplateEngine::evaluate(mkfileloc,cntxt);
	AfcUtil::writeTofile(mkfileamloc,ret,true);

	cntxt.clear();
#ifdef INC_SDORM_SQL
	cntxt["MOD_SDORM_SQL"] = "true";
#else
	cntxt["MOD_SDORM_SQL"] = "false";
#endif
#ifdef INC_SDORM_MONGO
	cntxt["MOD_SDORM_MONGO"] = "true";
#else
	cntxt["MOD_SDORM_MONGO"] = "false";
#endif
#ifdef INC_SCRH
	cntxt["MOD_SCRIPT"] = "true";
#else
	cntxt["MOD_SCRIPT"] = "false";
#endif

	std::string cffileloc = rtdcfpath+"/autotools/configure.ac.template";
	std::string cffileamloc = rtdcfpath+"/autotools/configure.ac";
	ret = TemplateEngine::evaluate(cffileloc,cntxt);
	AfcUtil::writeTofile(cffileamloc,ret,true);

	/*string configureFilePath = rtdcfpath+"/autotools/configure";
    if (access( configureFilePath.c_str(), F_OK ) == -1 )
	{
		std::string compres = rtdcfpath+"/autotools/autogen.sh "+serverRootDirectory;
		std::string output = ScriptHandler::execute(compres, true);
		logger << "Set up configure for intermediate libraries\n\n" << std::endl;
	}

	std::string compres = respath+"rundyn-configure.sh "+serverRootDirectory;
#ifdef DEBUG
	compres += " --enable-debug=yes";
#endif
	std::string output = ScriptHandler::execute(compres, true);
	logger << "Set up makefiles for intermediate libraries\n\n" << std::endl;
	logger << output << std::endl;*/
//#endif
}

void ConfigurationHandler::configureDataSources(const std::string& name, const std::string& configFile, std::map<std::string, ClassStructure>& allclsmap)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	XmlParser parser("Parser");
	logger << ("started reading sdorm config file " + configFile) << std::endl;

	Document doc;
	parser.readDocument(configFile, doc);
	const Element& dbroot = doc.getRootElement();

	std::map<std::string, ConnectionProperties> allProps;
	std::map<std::string, Mapping> allMapps;

	if(dbroot.getTagName()=="sdorm")
	{
		ElementList datasrcs = dbroot.getChildElements();
		bool found_default = false;
		for (unsigned int dsnu = 0; dsnu < datasrcs.size(); dsnu++)
		{
			if(datasrcs.at(dsnu).getTagName()=="data-source")
			{
				ConnectionProperties cprops;
				if(!found_default && (datasrcs.at(dsnu).getAttribute("default")=="true" || datasrcs.size()==1)) {
					cprops.addProperty("_isdefault_", "true");
					found_default = true;
				}
				Mapping mapping;
				mapping.appName = name;

				ElementList dbeles = datasrcs.at(dsnu).getChildElements();
				for (unsigned int dbs = 0; dbs < dbeles.size(); dbs++)
				{
					if(dbeles.at(dbs).getTagName()=="config")
					{
						ElementList confs = dbeles.at(dbs).getChildElements();
						int psize= 2;
						for (unsigned int cns = 0; cns < confs.size(); cns++)
						{
							if(confs.at(cns).getTagName()=="nodes")
							{
								ElementList nodec = confs.at(cns).getChildElements();
								for (unsigned int nn = 0; nn < nodec.size(); nn++)
								{
									if(nodec.at(nn).getTagName()!="node")continue;
									ConnectionNode cnode;
									ElementList nodes = nodec.at(nn).getChildElements();
									for (unsigned int ncc = 0; ncc < nodes.size(); ncc++)
									{
										if(nodes.at(ncc).getTagName()=="username")
										{
											cnode.username = nodes.at(ncc).getText();
										}
										else if(nodes.at(ncc).getTagName()=="password")
										{
											cnode.password = nodes.at(ncc).getText();
										}
										else if(nodes.at(ncc).getTagName()=="host")
										{
											cnode.host = nodes.at(ncc).getText();
										}
										else if(nodes.at(ncc).getTagName()=="port")
										{
											try {
												cnode.port = CastUtil::lexical_cast<int>(nodes.at(ncc).getText());
											} catch (...) {

											}
										}
										else if(nodes.at(ncc).getTagName()=="readTimeout")
										{
											try {
												cnode.readTimeout = CastUtil::lexical_cast<float>(nodes.at(ncc).getText());
											} catch (...) {

											}
										}
										else if(nodes.at(ncc).getTagName()=="connectionTimeout")
										{
											try {
												cnode.connectionTimeout = CastUtil::lexical_cast<float>(nodes.at(ncc).getText());
											} catch (...) {

											}
										}
										else if(nodes.at(ncc).getTagName()=="dsn")
										{
											cnode.dsn = nodes.at(ncc).getText();
										}
										else if(nodes.at(ncc).getTagName()=="databaseName")
										{
											cnode.databaseName = nodes.at(ncc).getText();
										}
									}
									cprops.addNode(cnode);
								}
							}
							else if(confs.at(cns).getTagName()=="pool-size")
							{
								if(confs.at(cns).getText()!="")
								{
									try {
										psize = CastUtil::lexical_cast<int>(confs.at(cns).getText());
									} catch (...) {

									}
								}
								cprops.poolWriteSize = psize;
							}
							else if(confs.at(cns).getTagName()=="name")
							{
								cprops.name = confs.at(cns).getText();
							}
							else if(confs.at(cns).getTagName()=="type")
							{
								cprops.type = confs.at(cns).getText();
							}
							else
							{
								cprops.addProperty(confs.at(cns).getTagName(), confs.at(cns).getText());
							}
						}
					}
					else if(dbeles.at(dbs).getTagName()=="tables")
					{
						ElementList tabs = dbeles.at(dbs).getChildElements();
						for (unsigned int dn = 0; dn < tabs.size(); dn++)
						{
							if(tabs.at(dn).getTagName()=="table")
							{
								DataSourceEntityMapping dsempg;
								dsempg.tableName = tabs.at(dn).getAttribute("name");
								dsempg.className = tabs.at(dn).getAttribute("class");
								ElementList cols = tabs.at(dn).getChildElements();
								for (unsigned int cn = 0; cn < cols.size(); cn++)
								{
									if(cols.at(cn).getTagName()=="hasOne")
									{
										DataSourceInternalRelation relation;
										relation.clsName = cols.at(cn).getText();
										relation.type = 1;
										relation.dfk = cols.at(cn).getAttribute("dfk");
										relation.dmappedBy = cols.at(cn).getAttribute("dmappedBy");
										relation.field = cols.at(cn).getAttribute("field");
										dsempg.addRelation(relation);
									}
									else if(cols.at(cn).getTagName()=="hasMany")
									{
										DataSourceInternalRelation relation;
										relation.clsName = cols.at(cn).getText();
										relation.type = 2;
										relation.dfk = cols.at(cn).getAttribute("dfk");
										relation.dmappedBy = cols.at(cn).getAttribute("dmappedBy");
										relation.field = cols.at(cn).getAttribute("field");
										dsempg.addRelation(relation);
									}
									else if(cols.at(cn).getTagName()=="col")
									{
										dsempg.addPropertyColumnMapping(cols.at(cn).getAttribute("obf"),
												cols.at(cn).getAttribute("dbf"));
									}
								}
								mapping.addDataSourceEntityMapping(dsempg);
							}
						}
					}
				}
				StringUtil::trim(cprops.name);
				if(cprops.name == "")
				{
					logger << "Data Source Name not defined, skipping.... " << std::endl;
					continue;
				}
				allProps[cprops.name] = cprops;
				allMapps[cprops.name] = mapping;
			}
		}
	}

	handleDataSourceEntities(name, allMapps, allclsmap);

	ConfigurationData::getInstance()->sdormConnProperties[name] = allProps;
	ConfigurationData::getInstance()->sdormEntityMappings[name] = allMapps;

	logger << "done reading sdorm config file " + configFile << std::endl;
}

void ConfigurationHandler::destroyDataSources()
{
	DataSourceManager::destroy();
}

void ConfigurationHandler::configureCaches(const std::string& name, const std::string& configFile)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	XmlParser parser("Parser");
	logger << ("started reading cache config file " + configFile) << std::endl;

	Document doc;
	parser.readDocument(configFile, doc);
	const Element& dbroot = doc.getRootElement();

	if(dbroot.getTagName()=="caches")
	{
		ElementList datasrcs = dbroot.getChildElements();
		bool found_default = false;
		for (unsigned int dsnu = 0; dsnu < datasrcs.size(); dsnu++)
		{
			if(datasrcs.at(dsnu).getTagName()=="cache")
			{
				ConnectionProperties cprops;
				if(!found_default && (datasrcs.at(dsnu).getAttribute("default")=="true" || datasrcs.size()==1)) {
					cprops.addProperty("_isdefault_", "true");
					found_default = true;
				}
				int psize = 1;
				ElementList confs = datasrcs.at(dsnu).getChildElements();
				for (unsigned int cns = 0; cns < confs.size(); cns++)
				{
					if(confs.at(cns).getTagName()=="nodes")
					{
						ElementList nodec = confs.at(cns).getChildElements();
						for (unsigned int nn = 0; nn < nodec.size(); nn++)
						{
							if(nodec.at(nn).getTagName()!="node")continue;
							ConnectionNode cnode;
							ElementList nodes = nodec.at(nn).getChildElements();
							for (unsigned int ncc = 0; ncc < nodes.size(); ncc++)
							{
								if(nodes.at(ncc).getTagName()=="username")
								{
									cnode.username = nodes.at(ncc).getText();
								}
								else if(nodes.at(ncc).getTagName()=="password")
								{
									cnode.password = nodes.at(ncc).getText();
								}
								else if(nodes.at(ncc).getTagName()=="host")
								{
									cnode.host = nodes.at(ncc).getText();
								}
								else if(nodes.at(ncc).getTagName()=="port")
								{
									try {
										cnode.port = CastUtil::lexical_cast<int>(nodes.at(ncc).getText());
									} catch (...) {

									}
								}
								else if(nodes.at(ncc).getTagName()=="readTimeout")
								{
									try {
										cnode.readTimeout = CastUtil::lexical_cast<float>(nodes.at(ncc).getText());
									} catch (...) {

									}
								}
								else if(nodes.at(ncc).getTagName()=="connectionTimeout")
								{
									try {
										cnode.connectionTimeout = CastUtil::lexical_cast<float>(nodes.at(ncc).getText());
									} catch (...) {

									}
								}
							}
							cprops.addNode(cnode);
						}
					}
					else if(confs.at(cns).getTagName()=="pool-size")
					{
						if(confs.at(cns).getText()!="")
						{
							try {
								psize = CastUtil::lexical_cast<int>(confs.at(cns).getText());
							} catch (...) {

							}
						}
						cprops.poolWriteSize = psize;
					}
					else if(confs.at(cns).getTagName()=="name")
					{
						cprops.name = confs.at(cns).getText();
					}
					else if(confs.at(cns).getTagName()=="type")
					{
						cprops.type = confs.at(cns).getText();
					}
					else
					{
						cprops.addProperty(confs.at(cns).getTagName(), confs.at(cns).getText());
					}
				}

				if(cprops.getName()!="")
				{
					ConfigurationData::getInstance()->cacheConnProperties[name][cprops.getName()] = cprops;
				}
			}
		}
	}

	logger << "done reading cache config file " + configFile << std::endl;
}

void ConfigurationHandler::destroyCaches()
{
	CacheManager::destroy();
}

void ConfigurationHandler::handleMarkerConfigurations(std::map<std::string, std::map<std::string, ClassStructure> >& clsstrucMaps, std::vector<WsDetails>& wsdvec, std::vector<bool>& stat, strVec& vecvp, strVec& pathvec, std::map<std::string, std::string>& ajintpthMap, std::map<std::string, std::string>& tpes, const std::string& serverRootDirectory, strVec& afcd, std::string& ajrt, Reflection& ref)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	std::map<std::string, std::map<std::string, ClassStructure> >::iterator it;
	for (it=clsstrucMaps.begin();it!=clsstrucMaps.end();++it) {
		bool isJsObjects = false;
		std::string appName = it->first;
		std::string defpath = serverRootDirectory + "/web/" + appName;
		std::map<std::string, ClassStructure>::iterator itt;
		for (itt=it->second.begin();itt!=it->second.end();++itt) {
			ClassStructure cs = itt->second;
			if(cs.markers.find("@SecurityProvider")!=cs.markers.end())
			{
				Marker secProv = cs.markers["@SecurityProvider"].at(cs.markers["@SecurityProvider"].size()-1);

				if(cs.markers["@SecurityProvider"].size()>1)
				{
					logger << ("Found more than one @SecurityProvider marker, only the last defined marker will be considered, ignoring others..") << std::endl;
				}

				std::string clas = cs.getFullyQualifiedClassName();
				std::string provider = clas;
				std::string url = secProv.getAttributeValue("url");
				std::string provName = secProv.getAttributeValue("providerName");
				std::string sessionTimeoutV = secProv.getAttributeValue("sessionTimeout");
				std::string scope = secProv.getAttributeValue("scope");
				std::string welcomefile = secProv.getAttributeValue("welcomefile");
				std::string userfld = secProv.getAttributeValue("usernamefld");
				std::string userfrom = secProv.getAttributeValue("usernamefrom");
				std::string passfld = secProv.getAttributeValue("passwordfld");
				std::string passfrom = secProv.getAttributeValue("passwordfrom");

				if(provName=="")
				{
					logger << ("Provider name cannot be blank, skipping...") <<  std::endl;
					continue;
				}

				if(url=="")
				{
					logger << ("Login Path cannot be blank, skipping...") <<  std::endl;
					continue;
				}

				if((userfld!="" && passfld=="") || (userfld=="" && passfld!=""))
				{
					logger << ("Both usernamefld/passwordfld need to be provided together, skipping...") <<  std::endl;
					continue;
				}

				if((userfld!="" && userfrom!="" && passfrom=="") || (passfld!="" && userfrom=="" && passfrom!=""))
				{
					logger << ("Both usernamefrom/passwordfrom need to be provided together, skipping...") <<  std::endl;
					continue;
				}

				if(ConfigurationData::getInstance()->securityObjectMap[appName].find(provName)
						!=ConfigurationData::getInstance()->securityObjectMap[appName].end())
				{
					logger << ("Provider with name " + provName + " already exists, skipping duplicate...") <<  std::endl;
					continue;
				}

				Security securityObject = ConfigurationData::getInstance()->securityObjectMap[appName][provName];
				securityObject.loginProvider = "class:" + provider;
				normalizeUrl(appName, url);
				securityObject.loginUrl = url;
				try {
					securityObject.sessTimeout = CastUtil::lexical_cast<long>(sessionTimeoutV);
				} catch (...) {
					securityObject.sessTimeout = 3600;
					logger << ("Security: Invalid session timeout value defined, defaulting to 1hour/3600sec") << std::endl;
				}

				securityObject.welcomeFile = welcomefile;
				securityObject.securityFieldNames["username"] = userfld;
				securityObject.securityFieldFrom["username"] = userfrom;
				securityObject.securityFieldNames["password"] = passfld;
				securityObject.securityFieldFrom["password"] = passfrom;

				ConfigurationData::getInstance()->securityObjectMap[appName][provName] = securityObject;
				Bean bean("login-handler_"+clas,"",clas,scope,false,appName);
				ConfigurationData::getInstance()->ffeadContext.addBean(bean);
				logger << ("Security: Adding Security Provider => " + url  + " , provider => " + provider) << std::endl;
			}
		}

		for (itt=it->second.begin();itt!=it->second.end();++itt) {
			ClassStructure cs = itt->second;
			if(cs.markers.find("@RestController")!=cs.markers.end())
			{
				handleRestControllerMarker(cs, appName);
			}
			else if(cs.markers.find("@WebService")!=cs.markers.end())
			{
				Marker webcnt = cs.markers["@WebService"].at(cs.markers["@WebService"].size()-1);
				if(cs.markers["@WebService"].size()>1)
				{
					logger << ("Found more than one @WebService marker, only the last defined marker will be considered, ignoring others..") << std::endl;
				}
				WsDetails wsd;
				wsd.claz = cs.getFullyQualifiedClassName();
				wsd.location = webcnt.getAttributeValue("location");
				if(StringUtil::trimCopy(wsd.location)=="")
				{
					logger << ("No location defined for web-service, skipping...") << std::endl;
					continue;
				}
				wsd.namespc = webcnt.getAttributeValue("namespc");
				/*if(StringUtil::trimCopy(wsd.namespc)=="")
				{
					logger << ("No namespace defined for web-service, skipping...") << std::endl;
					continue;
				}*/
				std::map<std::string, std::string> outnmmp;
				if(cs.markers.find("@Secure")!=cs.markers.end())
				{
					Marker securr = cs.markers["@Secure"].at(cs.markers["@Secure"].size()-1);
					SecureAspect aspect;
					aspect.role = securr.getAttributeValue("role");
					bool valid = true;
					if(StringUtil::trimCopy(aspect.role)=="")
					{
						logger << ("No role defined for Secure path, skipping...") << std::endl;
						valid = false;
					}
					std::string provName = securr.getAttributeValue("providerName");
					if(StringUtil::trimCopy(provName)=="")
					{
						logger << ("No providerName defined for Secure path, skipping...") << std::endl;
						valid = false;
					}
					if(cs.markers["@Secure"].size()>1)
					{
						logger << ("Found more than one @Secure marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}
					aspect.path = wsd.location;
					normalizeUrl(appName, aspect.path);

					if(valid && ConfigurationData::getInstance()->securityObjectMap[appName].find(provName)
							!=ConfigurationData::getInstance()->securityObjectMap[appName].end())
					{
						Security securityObject = ConfigurationData::getInstance()->securityObjectMap[appName][provName];
						if(securityObject.addAspect(aspect))
						{
							securityObject.addAspect(aspect);
							ConfigurationData::getInstance()->securityObjectMap[appName][provName] = securityObject;
							logger << ("Security: Adding Secure Path => " + aspect.path  + " , role => " + aspect.role) << std::endl;
						}
					}
					else if(valid)
					{
						logger << ("Security Provider " + provName + " not found, skipping...") << std::endl;
					}
				}
				for (int var = 0; var < (int)cs.pubms.size(); ++var) {
					if(cs.pubms.at(var).markers.find("@WebServiceMethod")!=cs.pubms.at(var).markers.end())
					{
						Marker websrvmth = cs.pubms.at(var).markers["@WebServiceMethod"].at(cs.pubms.at(var).markers["@WebServiceMethod"].size()-1);
						if(cs.pubms.at(var).markers["@WebServiceMethod"].size()>1)
						{
							logger << ("Found more than one @WebServiceMethod marker, only the last defined marker will be considered, ignoring others..") << std::endl;
						}
						outnmmp[cs.pubms.at(var).name] = websrvmth.getAttributeValue("outname");
					}
				}
				wsd.outnmmp = outnmmp;
				wsd.appname = appName;
				wsdvec.push_back(wsd);
			}
			else if(cs.markers.find("@Controller")!=cs.markers.end())
			{
				std::vector<Marker> controllers = cs.markers["@Controller"];
				for (int var = 0; var < (int)controllers.size(); ++var) {
					std::string url = controllers.at(var).getAttributeValue("path");
					StringUtil::trim(url);
					if(url=="")
					{
						logger << ("No url defined for Controller, skipping...") << std::endl;
						continue;
					}
					std::string scope = controllers.at(var).getAttributeValue("scope");
					StringUtil::trim(scope);
					std::string clas = cs.getFullyQualifiedClassName();
					StringUtil::trim(clas);
					if(clas!="")
					{
						normalizeUrl(appName, url);
						ConfigurationData::getInstance()->controllerObjectMap[appName][url] = clas;
						Bean bean("controller_"+clas,"",clas,scope,false,appName);
						ConfigurationData::getInstance()->ffeadContext.addBean(bean);
					}
					logger << ("Adding Controller for " + (appName + url) + " :: " + clas) << std::endl;
				}
			}
			else if(cs.markers.find("@AjaxInterface")!=cs.markers.end())
			{
				std::vector<Marker> ajaxintfs = cs.markers["@AjaxInterface"];
				for (int var = 0; var < (int)ajaxintfs.size(); ++var) {
					std::string url = ajaxintfs.at(var).getAttributeValue("path");
					if(StringUtil::trimCopy(url)=="")
					{
						logger << ("No path defined for AjaxInterface, skipping...") << std::endl;
						continue;
					}
					if(cs.markers.find("@Secure")!=cs.markers.end())
					{
						int totl = cs.markers["@Secure"].size();
						SecureAspect aspect;
						aspect.role = cs.markers["@Secure"].at(totl-1).getAttributeValue("role");
						aspect.path = url;
						normalizeUrl(appName, aspect.path);
						bool valid = true;
						if(StringUtil::trimCopy(aspect.role)=="")
						{
							logger << ("No role defined for Secure path, skipping...") << std::endl;
							valid = false;
						}
						if(cs.markers["@Secure"].size()>1)
						{
							logger << ("Found more than one @Secure marker, only the last defined marker will be considered, ignoring others..") << std::endl;
						}
						std::string provName = cs.markers["@Secure"].at(totl-1).getAttributeValue("providerName");
						if(StringUtil::trimCopy(provName)=="")
						{
							logger << ("No providerName defined for Secure path, skipping...") << std::endl;
							valid = false;
						}
						if(valid && ConfigurationData::getInstance()->securityObjectMap[appName].find(provName)
								!=ConfigurationData::getInstance()->securityObjectMap[appName].end())
						{
							Security securityObject = ConfigurationData::getInstance()->securityObjectMap[appName][provName];
							if(securityObject.addAspect(aspect))
							{
								ConfigurationData::getInstance()->securityObjectMap[appName][provName] = securityObject;
								logger << ("Security: Adding Secure Path => " + aspect.path  + " , role => " + aspect.role) << std::endl;
							}
						}
						else if(valid)
						{
							logger << ("Security Provider " + provName + " not found, skipping...") << std::endl;
						}
					}
					if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
					{
						normalizeUrl(appName, url);
						std::string clas = cs.getFullyQualifiedClassName();
						ConfigurationData::getInstance()->ajaxInterfaceMap[appName][url] = clas;
						pathvec.push_back(appName);
						std::string usrincludes = defpath + "include/";
						vecvp.push_back(usrincludes);
						stat.push_back(false);
						ajintpthMap[clas] = url;
						afcd.push_back(clas);
						logger << ("Adding Ajax Interface for " + (appName+url) + " :: " + clas) << std::endl;
						isJsObjects = true;
					}
					else
					{
						logger << ("Skipping Ajax Interface as path contains */regex function") << std::endl;
					}
				}
			}
			else if(cs.markers.find("@Filter")!=cs.markers.end())
			{
				std::vector<Marker> filters = cs.markers["@Filter"];
				for (int var = 0; var < (int)filters.size(); ++var) {
					std::string url = filters.at(var).getAttributeValue("path");
					if(StringUtil::trimCopy(url)=="")
					{
						logger << ("No path defined for Filter, skipping...") << std::endl;
						continue;
					}
					std::string clas = cs.getFullyQualifiedClassName();
					std::string type = filters.at(var).getAttributeValue("type");
					std::string scope = filters.at(var).getAttributeValue("scope");
					if(clas!="" && (type=="in" || type=="out" || type=="handle"))
					{
						normalizeUrl(appName, url);
						if(url=="")url="*.*";
						if(clas!="")
						{
							ConfigurationData::getInstance()->filterObjectMap[appName][url+type].push_back(clas);
							Bean bean("filter_"+clas,"",clas,scope,false,appName);
							ConfigurationData::getInstance()->ffeadContext.addBean(bean);
						}
						logger << ("Adding Filter for " + (appName + url + type) + " :: " + clas) << std::endl;
					}
				}
			}
			else if(cs.markers.find("@Template")!=cs.markers.end())
			{
				std::vector<Marker> templates = cs.markers["@Template"];
				for (int var = 0; var < (int)templates.size(); ++var) {
					std::string clas = cs.getFullyQualifiedClassName();
					std::string url = templates.at(var).getAttributeValue("path");
					std::string file = StringUtil::trimCopy(templates.at(var).getAttributeValue("file"));
					if(StringUtil::trimCopy(url)=="")
					{
						logger << ("No path defined for Template, skipping...") << std::endl;
						continue;
					}
					if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
					{
						normalizeUrl(appName, url);
						std::string fpath = defpath+"/tpe/"+file;
						RegexUtil::replace(fpath,"[/]+","/");
						std::string wbpi = fpath;
						StringUtil::replaceFirst(wbpi, ConfigurationData::getInstance()->coreServerProperties.webPath, "");
						RegexUtil::replace(wbpi,"[^a-zA-Z0-9_]+","");
						ConfigurationData::getInstance()->templateMappingMap[appName][url] = clas+";"+wbpi;
						tpes[fpath] = appName;
						//tpes.push_back(defpath+tmplts.at(tmpn).getAttribute("file"));
						std::string scope = templates.at(var).getAttributeValue("scope");
						if(clas!="")
						{
							Bean bean("template_"+clas,"",clas,scope,false,appName);
							ConfigurationData::getInstance()->ffeadContext.addBean(bean);
						}
						logger << ("Adding Template for " + (url) + " :: " + clas) << std::endl;
					}
					else
					{
						logger << ("Skipping Template as path contains */regex function") << std::endl;
					}
				}
			}
			else if(cs.markers.find("@WebSocketHandler")!=cs.markers.end())
			{
				std::vector<Marker> templates = cs.markers["@WebSocketHandler"];
				for (int var = 0; var < (int)templates.size(); ++var) {
					std::string clas = cs.getFullyQualifiedClassName();
					std::string url = StringUtil::trimCopy(templates.at(var).getAttributeValue("path"));
					if(StringUtil::trimCopy(url)=="")
					{
						logger << ("No path defined for WebSocketHandler, skipping...") << std::endl;
						continue;
					}
					if(url.find("*")==std::string::npos && url.find("regex(")==std::string::npos)
					{
						normalizeUrl(appName, url);
						ConfigurationData::getInstance()->websocketMappingMap[appName][url] = clas;
						std::string scope = templates.at(var).getAttributeValue("scope");
						if(clas!="")
						{
							Bean bean("websocketclass_"+clas,"",clas,scope,false,appName);
							ConfigurationData::getInstance()->ffeadContext.addBean(bean);
						}
						logger << ("Adding WebSocketHandler for " + (url) + " :: " + clas) << std::endl;
					}
					else
					{
						logger << ("Skipping WebSocketHandler as path contains */regex function") << std::endl;
					}
				}
			}
			else if(cs.markers.find("@Dview")!=cs.markers.end())
			{
				std::vector<Marker> dviews = cs.markers["@Dview"];
				for (int var = 0; var < (int)dviews.size(); ++var) {
					std::string clas = cs.getFullyQualifiedClassName();
					std::string path = dviews.at(var).getAttributeValue("path");
					if(StringUtil::trimCopy(path)=="")
					{
						logger << ("No path defined for Dview, skipping...") << std::endl;
						continue;
					}
					std::string url = path;
					normalizeUrl(appName, url);
					ConfigurationData::getInstance()->viewMappingMap[appName][url] = clas;
					std::string scope = dviews.at(var).getAttributeValue("scope");
					if(clas!="")
					{
						Bean bean("dview_"+clas,"",clas,scope,false,appName);
						ConfigurationData::getInstance()->ffeadContext.addBean(bean);
					}
					logger << ("Adding Dynamic View for " + (appName+url) + " :: " + clas) << std::endl;
				}
			}
		}

		if(isJsObjects)
		{
			std::string infjs;
			ajrt += AfcUtil::generateJsInterfacessAll(it->second,infjs,ajintpthMap,afcd,ref);
			std::string objs = AfcUtil::generateJsObjectsAll(it->second);
			vecvp.clear();
			afcd.clear();
			pathvec.clear();
			ajintpthMap.clear();
			std::string webpubpath = serverRootDirectory + "/web/" + appName + "/public/";

			AfcUtil::writeTofile(webpubpath+"_afc_Objects.js",objs,true);
			AfcUtil::writeTofile(webpubpath+"_afc_Interfaces.js",infjs,true);
		}
	}
}

void ConfigurationHandler::handleDataSourceEntities(const std::string& appName, std::map<std::string, Mapping>& mappings, std::map<std::string, ClassStructure>& allclsmap)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");

	std::map<std::string, ClassStructure>::iterator itt;
	for (itt=allclsmap.begin();itt!=allclsmap.end();++itt) {
		ClassStructure cs = itt->second;
		std::string clas = cs.getFullyQualifiedClassName();
		if(cs.markers.find("@Entity")!=cs.markers.end() && cs.markers.find("@Table")!=cs.markers.end())
		{
			//std::cout << "Entity " << clas << std::endl;
			DataSourceEntityMapping dsempg;
			dsempg.className = clas;
			std::vector<Marker> entityv = cs.markers["@Entity"];
			std::vector<Marker> tablev = cs.markers["@Table"];

			if(entityv.size()>1)
			{
				logger << ("Found more than one @Entity marker, only the last defined marker will be considered, ignoring others..") << std::endl;
			}

			if(tablev.size()>1)
			{
				logger << ("Found more than one @Entity marker, only the last defined marker will be considered, ignoring others..") << std::endl;
			}

			Marker mentity = entityv.at(entityv.size()-1);
			Marker mtable = tablev.at(tablev.size()-1);

			std::string tablename = mtable.getAttributeValue("name");
			if(StringUtil::trimCopy(tablename)=="")
			{
				logger << ("No table name defined for Table, skipping...") << std::endl;
				continue;
			}
			std::string dataSourceName = mentity.getAttributeValue("dataSourceName");
			dsempg.tableName = tablename;
			bool idfound = false;
			bool skipIt = false;
			std::vector<PropStructure> csaps = cs.getAllProps();
			for (int var = 0; var < (int)csaps.size(); ++var) {
				//std::cout << "Property " << csaps.at(var).name << std::endl;
				std::map<std::string, std::vector<Marker> >::iterator piit;
				for (piit=csaps.at(var).markers.begin();piit!=csaps.at(var).markers.end();++piit) {
					std::vector<Marker> miit = piit->second;
					for(int mi=0;mi<(int)miit.size();mi++) {
						//std::cout << miit.at(mi).name << std::endl;
					}
				}
				if(csaps.at(var).markers.find("@Column")!=csaps.at(var).markers.end())
				{
					if(csaps.at(var).markers["@Column"].size()>1)
					{
						logger << ("Found more than one @Column marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}
					Marker colmrk = csaps.at(var).markers["@Column"].at(csaps.at(var).markers["@Column"].size()-1);
					std::string dbf = colmrk.getAttributeValue("dbf");
					if(dbf=="")dbf = csaps.at(var).name;
					dsempg.addPropertyColumnMapping(csaps.at(var).name, dbf);
				}
				else if(csaps.at(var).markers.find("@Id")!=csaps.at(var).markers.end())
				{
					if(csaps.at(var).markers["@Id"].size()>1)
					{
						logger << ("Found more than one @Id marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}
					if(!idfound) {
						Marker colmrk = csaps.at(var).markers["@Id"].at(csaps.at(var).markers["@Id"].size()-1);
						std::string dbf = colmrk.getAttributeValue("dbf");
						if(dbf=="")dbf = csaps.at(var).name;
						dsempg.addPropertyColumnMapping(csaps.at(var).name, dbf);
						dsempg.idPropertyName = csaps.at(var).name;
						idfound = true;
						if(csaps.at(var).markers.find("@IdGenerate")!=csaps.at(var).markers.end())
						{
							if(csaps.at(var).markers["@IdGenerate"].size()>1)
							{
								logger << ("Found more than one @IdGenerate marker, only the last defined marker will be considered, ignoring others..") << std::endl;
							}
							colmrk = csaps.at(var).markers["@IdGenerate"].at(csaps.at(var).markers["@IdGenerate"].size()-1);
							dsempg.idgendbEntityType = colmrk.getAttributeValue("dbEntityType");
							dsempg.idgendbEntityName = colmrk.getAttributeValue("dbEntityName");
							dsempg.idgentype = colmrk.getAttributeValue("type");
							dsempg.idgenhiValueColumn = colmrk.getAttributeValue("hiValueColumn");
							dsempg.idgenlowValue = CastUtil::lexical_cast<int>(colmrk.getAttributeValue("lowValue"));
							dsempg.idgenentityColumn = colmrk.getAttributeValue("entityColumn");
							dsempg.idgencolumnName = colmrk.getAttributeValue("columnName");
							if(dsempg.idgencolumnName=="")dsempg.idgencolumnName = csaps.at(var).name;
						}
					} else {
						logger << ("Multiple Id properties defined, skipping...") << std::endl;
						skipIt = true;
						break;
					}
				}

				if(csaps.at(var).markers.find("@HasOne")!=csaps.at(var).markers.end())
				{
					if(csaps.at(var).markers["@HasOne"].size()>1)
					{
						logger << ("Found more than one @HasOne marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}
					Marker colmrk = csaps.at(var).markers["@HasOne"].at(csaps.at(var).markers["@HasOne"].size()-1);
					std::string dfk = colmrk.getAttributeValue("dfk");
					std::string dmappedBy = colmrk.getAttributeValue("dmappedBy");
					if(StringUtil::trimCopy(dfk)=="" || StringUtil::trimCopy(dmappedBy)=="")
					{
						logger << ("No dfk/dmappedBy defined for HasOne marker, both need to be defined, skipping...") << std::endl;
						continue;
					}
					DataSourceInternalRelation relation;
					relation.clsName = csaps.at(var).type;
					relation.type = 1;
					relation.dfk = dfk;
					relation.dmappedBy = dmappedBy;
					relation.field = csaps.at(var).name;
					dsempg.addRelation(relation);
				}
				else if(csaps.at(var).markers.find("@HasMany")!=csaps.at(var).markers.end())
				{
					if(csaps.at(var).markers["@HasMany"].size()>1)
					{
						logger << ("Found more than one @HasMany marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}
					Marker colmrk = csaps.at(var).markers["@HasMany"].at(csaps.at(var).markers["@HasMany"].size()-1);
					std::string dfk = colmrk.getAttributeValue("dfk");
					std::string dmappedBy = colmrk.getAttributeValue("dmappedBy");
					if(StringUtil::trimCopy(dfk)=="" || StringUtil::trimCopy(dmappedBy)=="")
					{
						logger << ("No dfk/dmappedBy defined for HasMany marker, both need to be defined, skipping...") << std::endl;
						continue;
					}

					std::string mtype = csaps.at(var).type;
					if(mtype.find("vector<")==std::string::npos || mtype.find("std::vector<")==std::string::npos)
					{
						logger << ("HasMany association can have only a stl vector mapping, skipping...") << std::endl;
						continue;
					}

					StringUtil::replaceFirst(mtype,"std::","");
					StringUtil::replaceFirst(mtype,"vector<","");
					if(mtype.find(",")!=std::string::npos)
						mtype = mtype.substr(0,mtype.find(","));
					else if(mtype.find(">")!=std::string::npos)
						mtype = mtype.substr(0,mtype.find(">"));

					DataSourceInternalRelation relation;
					relation.clsName = mtype;
					relation.type = 2;
					relation.dfk = dfk;
					relation.dmappedBy = dmappedBy;
					relation.field = csaps.at(var).name;
					dsempg.addRelation(relation);
				}
			}
			if(!skipIt)
			{
				if(idfound)
				{
					if(mappings.find(dataSourceName)!=mappings.end() && mappings[dataSourceName].appName==appName)
					{
						Mapping mp = mappings[dataSourceName];
						mp.addDataSourceEntityMapping(dsempg);
						mappings[dataSourceName] = mp;
						//std::cout << "adding mapping for dsn " << dataSourceName << ", app is " << appName << std::endl;
					}
					else
					{
						std::map<std::string, Mapping>::iterator it;
						for(it=mappings.begin();it!=mappings.end();++it)
						{
							if(it->second.appName==appName)
							{
								//std::cout << "adding mapping for dsn " << it->first << ", app is " << appName << std::endl;
								Mapping mp = it->second;
								mp.addDataSourceEntityMapping(dsempg);
								mappings[it->first] = mp;
							}
						}
					}
				}
				else
				{
					logger << ("No Id column specified, skipping...") << std::endl;
					continue;
				}
			}
		}
	}
}

Marker ConfigurationHandler::getRestFunctionMarker(std::map<std::string, std::vector<Marker> >& markers)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	if(markers.find("@GET")!=markers.end())
	{
		if(markers["@GET"].size()>1)
		{
			logger << ("Found more than one @GET marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@GET"].at(markers["@GET"].size()-1);
	}
	else if(markers.find("@POST")!=markers.end())
	{
		if(markers["@POST"].size()>1)
		{
			logger << ("Found more than one @POST marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@POST"].at(markers["@POST"].size()-1);
	}
	else if(markers.find("@PUT")!=markers.end())
	{
		if(markers["@PUT"].size()>1)
		{
			logger << ("Found more than one @PUT marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@PUT"].at(markers["@PUT"].size()-1);
	}
	else if(markers.find("@DELETE")!=markers.end())
	{
		if(markers["@DELETE"].size()>1)
		{
			logger << ("Found more than one @DELETE marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@DELETE"].at(markers["@DELETE"].size()-1);
	}
	return Marker();
}

Marker ConfigurationHandler::getRestFunctionParamMarker(std::map<std::string, std::vector<Marker> >& markers)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	if(markers.find("@PathParam")!=markers.end())
	{
		if(markers["@PathParam"].size()>1)
		{
			logger << ("Found more than one @PathParam marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@PathParam"].at(markers["@PathParam"].size()-1);
	}
	else if(markers.find("@QueryParam")!=markers.end())
	{
		if(markers["@QueryParam"].size()>1)
		{
			logger << ("Found more than one @QueryParam marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@QueryParam"].at(markers["@QueryParam"].size()-1);
	}
	else if(markers.find("@FormParam")!=markers.end())
	{
		if(markers["@FormParam"].size()>1)
		{
			logger << ("Found more than one @FormParam marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@FormParam"].at(markers["@FormParam"].size()-1);
	}
	else if(markers.find("@HeaderParam")!=markers.end())
	{
		if(markers["@HeaderParam"].size()>1)
		{
			logger << ("Found more than one @HeaderParam marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@HeaderParam"].at(markers["@HeaderParam"].size()-1);
	}
	else if(markers.find("@Body")!=markers.end())
	{
		if(markers["@Body"].size()>1)
		{
			logger << ("Found more than one @Body marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@Body"].at(markers["@Body"].size()-1);
	}
	else if(markers.find("@MultipartContent")!=markers.end())
	{
		if(markers["@MultipartContent"].size()>1)
		{
			logger << ("Found more than one @MultipartContent marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		return markers["@MultipartContent"].at(markers["@MultipartContent"].size()-1);
	}
	return Marker();
}

void ConfigurationHandler::handleRestControllerMarker(ClassStructure& cs, const std::string& appName)
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	Marker restcnt = cs.markers["@RestController"].at(cs.markers["@RestController"].size()-1);
	if(cs.markers["@RestController"].size()>1)
	{
		logger << ("Found more than one @RestController marker, only the last defined marker will be considered, ignoring others..") << std::endl;
	}
	std::string url = restcnt.getAttributeValue("path");
	std::string scope = restcnt.getAttributeValue("scope");
	std::string clas = cs.getFullyQualifiedClassName();
	StringUtil::trim(url);

	bool isClasSecure = false;
	if(cs.markers.find("@Secure")!=cs.markers.end())
	{
		int totl = cs.markers["@Secure"].size();
		SecureAspect aspect;
		aspect.role = cs.markers["@Secure"].at(totl-1).getAttributeValue("role");
		aspect.path = url;
		bool valid = true;
		if(StringUtil::trimCopy(aspect.role)=="")
		{
			logger << ("No role defined for Secure path, skipping...") << std::endl;
			valid = false;
		}
		normalizeUrl(appName, aspect.path);
		if(cs.markers["@Secure"].size()>1)
		{
			logger << ("Found more than one @Secure marker, only the last defined marker will be considered, ignoring others..") << std::endl;
		}
		std::string provName = cs.markers["@Secure"].at(totl-1).getAttributeValue("providerName");
		if(StringUtil::trimCopy(provName)=="")
		{
			logger << ("No providerName defined for Secure path, skipping...") << std::endl;
			valid = false;
		}
		if(valid && ConfigurationData::getInstance()->securityObjectMap[appName].find(provName)
				!=ConfigurationData::getInstance()->securityObjectMap[appName].end())
		{
			Security securityObject = ConfigurationData::getInstance()->securityObjectMap[appName][provName];
			if(securityObject.addAspect(aspect))
			{
				ConfigurationData::getInstance()->securityObjectMap[appName][provName] = securityObject;
				isClasSecure = true;
				logger << ("Security: Adding Secure Path => " + aspect.path  + " , role => " + aspect.role) << std::endl;
			}
		}
		else if(valid)
		{
			logger << ("Security Provider " + provName + " not found, skipping...") << std::endl;
		}
	}

	for (int var = 0; var < (int)cs.pubms.size(); ++var) {
		Marker m = getRestFunctionMarker(cs.pubms.at(var).markers);
		if(m.getName()!="")
		{
			RestFunction restfunction;
			restfunction.name = cs.pubms.at(var).name;
			restfunction.path = m.getAttributeValue("path");
			restfunction.statusCode = m.getAttributeValue("statusCode");
			if(restfunction.statusCode!="")
			{
				try {
					CastUtil::lexical_cast<int>(restfunction.statusCode);
				} catch(...) {
					logger << "Rest: invalid response statusCode specified, defaulting to 200.." << std::endl;
					restfunction.statusCode = "200";
				}
			}
			restfunction.clas = clas;
			restfunction.meth = StringUtil::toUpperCopy(m.getName().substr(1));
			restfunction.icontentType = m.getAttributeValue("icontentType");
			restfunction.ocontentType = m.getAttributeValue("ocontentType");

			std::map<int, std::map<std::string, std::vector<Marker> > > argMarkers = cs.pubms.at(var).argMarkers;
			std::map<int, std::map<std::string, std::vector<Marker> > >::iterator pit;
			bool hasBodyParam = false, invalidParam = false;
			for (pit=argMarkers.begin();pit!=argMarkers.end();++pit)
			{
				Marker rpm = getRestFunctionParamMarker(pit->second);
				if(rpm.getName()=="")
					continue;

				RestFunctionParams param;
				param.type = cs.pubms.at(var).argstypes[pit->first];
				StringUtil::replaceAll(param.type, " ", "");

				if(param.type.find("std::vector<")==0 || param.type.find("vector<")==0
						|| param.type.find("std::list<")==0 || param.type.find("list<")==0
						|| param.type.find("std::deque<")==0 || param.type.find("deque<")==0
						|| param.type.find("std::queue<")==0 || param.type.find("queue<")==0
						|| param.type.find("std::set<")==0 || param.type.find("set<")==0
						|| param.type.find("std::multiset<")==0 || param.type.find("multiset<")==0)
				{
					std::string tmpltyp = param.type.substr(0, param.type.find("<"));
					std::string typ = param.type;
					StringUtil::replaceFirst(typ, tmpltyp+"<", "");
					StringUtil::replaceFirst(typ, ">", "");

					StringUtil::replaceFirst(tmpltyp, "std::", "");

					if(typ=="ifstream*" || typ=="std::ifstream*")
						param.type = tmpltyp + "-of-filestream";
					else
						param.type = tmpltyp + "-of-" + typ;
				}

				if(param.type=="ifstream*" || param.type=="std::ifstream*")
					param.type = "filestream";

				//TODO set deque set multiset list

				if(rpm.getName()=="@PathParam")
					param.from = "path";
				else if(rpm.getName()=="@FormParam")
					param.from = "postparam";
				else if(rpm.getName()=="@QueryParam")
					param.from = "reqparam";
				else if(rpm.getName()=="@HeaderParam")
					param.from = "header";
				else if(rpm.getName()=="@MultipartContent")
					param.from = "multipart-content";
				else if(rpm.getName()=="@Body")
					param.from = "body";

				param.name = rpm.getAttributeValue("name");

				if(pit->second.find("@DefaultValue")!=pit->second.end())
				{
					std::vector<Marker> defmarkers = pit->second["@DefaultValue"];
					if(defmarkers.size()>1)
					{
						logger << ("Found more than one @DefaultValue marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}
					if(defmarkers.at(defmarkers.size()-1).getAttributeValue("value")!="")
					{
						param.defValue = defmarkers.at(defmarkers.size()-1).getAttributeValue("value");
					}
				}

				if(param.from=="body")
				{
					hasBodyParam = true;
				}
				if(StringUtil::trimCopy(param.type)=="")
				{
					invalidParam = true;
					logger << "Rest: no type specified for param" << std::endl;
				}
				else if((param.type=="filestream" || param.type=="vector-of-filestream") && param.from!="multipart-content")
				{

				}
				else if(param.type=="vector-of-filestream" && param.from=="multipart-content"
						&& StringUtil::trimCopy(param.name)=="")
				{

				}
				else if(param.from!="body" && StringUtil::trimCopy(param.name)=="")
				{
					invalidParam = true;
					logger << "Rest: no name specified for param" << std::endl;
				}
				else if(param.from=="body" && (restfunction.meth=="GET" || restfunction.meth=="OPTIONS" || restfunction.meth=="TRACE"
						|| restfunction.meth=="HEAD"))
				{
					invalidParam = true;
					logger << ("Rest: skipping param " + param.type + ", from is body and method is " + restfunction.meth) << std::endl;
				}
				else if(!(param.type=="int" || param.type=="short" || param.type=="long" || param.type=="float" || param.type=="string"
						|| param.type=="long long" || param.type=="std::string" || param.type=="double" || param.type=="bool"
						|| param.type=="filestream" || param.type=="vector-of-filestream") && param.from!="body")
				{
					invalidParam = true;
					logger << ("Rest: skipping param " + param.type + ", from is not body and input is a complex type") << std::endl;
				}
				else if(param.from=="postparam" && (restfunction.meth=="GET" || restfunction.meth=="OPTIONS" || restfunction.meth=="TRACE"
						|| restfunction.meth=="HEAD"))
				{
					invalidParam = true;
					logger << ("Rest: skipping param " + param.type + ", from is postparam and method is " + restfunction.meth) << std::endl;
				}
				if(!invalidParam)
				{
					restfunction.params.push_back(param);
				}
			}

			if(hasBodyParam && restfunction.params.size()>1)
			{
				logger << ("Rest: skipping method " + restfunction.meth + " as only one argument allowed of type body") << std::endl;
				continue;
			}
			if(invalidParam)
			{
				continue;
			}
			if(clas!="" && url.find("*")==std::string::npos)
			{
				if(url=="")
					url = "/" + clas;
				else if(url.at(0)!='/')
					url = "/" + url;
				std::string urlmpp;
				if(restfunction.path!="")
				{
					urlmpp = "/"+appName+url+"/"+restfunction.path;
					RegexUtil::replace(urlmpp,"[/]+","/");
					StringUtil::trim(urlmpp);
					if(urlmpp.at(urlmpp.length()-1)=='/') {
						urlmpp = urlmpp.substr(0, urlmpp.length()-1);
					}
					restfunction.path = urlmpp;
					ConfigurationData::getInstance()->rstCntMap[appName][restfunction.meth+urlmpp].push_back(restfunction);
				}
				else
				{
					urlmpp = "/"+appName+url+"/"+restfunction.name;
					RegexUtil::replace(urlmpp,"[/]+","/");
					StringUtil::trim(urlmpp);
					if(urlmpp.at(urlmpp.length()-1)=='/') {
						urlmpp = urlmpp.substr(0, urlmpp.length()-1);
					}
					restfunction.path = urlmpp;
					ConfigurationData::getInstance()->rstCntMap[appName][restfunction.meth+urlmpp].push_back(restfunction);
				}
				Bean bean("restcontroller_"+clas,"",clas,scope,false,appName);
				ConfigurationData::getInstance()->ffeadContext.addBean(bean);
				if(!isClasSecure && cs.pubms.at(var).markers.find("@Secure")!=cs.pubms.at(var).markers.end())
				{
					int totl = cs.pubms.at(var).markers["@Secure"].size();
					SecureAspect aspect;
					aspect.role = cs.pubms.at(var).markers["@Secure"].at(totl-1).getAttributeValue("role");
					bool valid = true;
					if(StringUtil::trimCopy(aspect.role)=="")
					{
						logger << ("No role defined for Secure path, skipping...") << std::endl;
						valid = false;
					}
					aspect.path = urlmpp;
					normalizeUrl(appName, aspect.path);
					std::string provName = cs.pubms.at(var).markers["@Secure"].at(totl-1).getAttributeValue("providerName");
					if(StringUtil::trimCopy(provName)=="")
					{
						logger << ("No providerName defined for Secure path, skipping...") << std::endl;
						valid = false;
					}

					if(cs.pubms.at(var).markers["@Secure"].size()>1)
					{
						logger << ("Found more than one @Secure marker, only the last defined marker will be considered, ignoring others..") << std::endl;
					}

					if(valid && ConfigurationData::getInstance()->securityObjectMap[appName].find(provName)
							!=ConfigurationData::getInstance()->securityObjectMap[appName].end())
					{
						Security securityObject = ConfigurationData::getInstance()->securityObjectMap[appName][provName];
						if(securityObject.addAspect(aspect))
						{
							ConfigurationData::getInstance()->securityObjectMap[appName][provName] = securityObject;
							logger << ("Security: Adding Secure Path => " + aspect.path  + " , role => " + aspect.role) << std::endl;
						}
					}
					else if(valid)
					{
						logger << ("Security Provider " + provName + " not found, skipping...") << std::endl;
					}
				}
				logger << ("Rest: Adding rest-controller => " + urlmpp  + " , class => " + clas) << std::endl;
			}
		}
	}
}

void ConfigurationHandler::initializeDataSources()
{
	Logger logger = LoggerFactory::getLogger("ConfigurationHandler");
	std::map<std::string, bool> mycntxts = ConfigurationData::getInstance()->servingContexts;
	std::map<std::string, std::map<std::string, ConnectionProperties> > sdormConnProperties = ConfigurationData::getInstance()->sdormConnProperties;
	std::map<std::string, std::map<std::string, Mapping> > sdormEntityMappings = ConfigurationData::getInstance()->sdormEntityMappings;
	std::map<std::string, bool>::iterator mit;
	for (mit=mycntxts.begin();mit!=mycntxts.end();++mit) {
		std::map<std::string, ConnectionProperties>::iterator it;
		if(sdormConnProperties.find(mit->first)!=sdormConnProperties.end())
		{
			logger << "app for ds is " << mit->first << std::endl;
			std::map<std::string, ConnectionProperties> allProps = sdormConnProperties[mit->first];
			std::map<std::string, Mapping> allMapps = sdormEntityMappings[mit->first];
			for (it=allProps.begin();it!=allProps.end();++it) {
				logger << "ds is " << it->first << std::endl;
				DataSourceManager::initDSN(allProps[it->first], allMapps[it->first]);
			}
		}
	}
}

void ConfigurationHandler::initializeCaches()
{
	std::map<std::string, bool> mycntxts = ConfigurationData::getInstance()->servingContexts;
	std::map<std::string, std::map<std::string, ConnectionProperties> > cacheConnProperties = ConfigurationData::getInstance()->cacheConnProperties;
	std::map<std::string, bool>::iterator mit;
	for (mit=mycntxts.begin();mit!=mycntxts.end();++mit) {
		if(cacheConnProperties.find(mit->first)!=cacheConnProperties.end())
		{
			std::map<std::string, ConnectionProperties> allProps = cacheConnProperties[mit->first];
			std::map<std::string, ConnectionProperties>::iterator it;
			for (it=allProps.begin();it!=allProps.end();++it) {
				CacheManager::initCache(allProps[it->first], mit->first);
			}
		}
	}
}

void ConfigurationHandler::initializeWsdls()
{
	Reflection ref;
	WsUtil wsu;
	std::map<std::string, bool> mycntxts = ConfigurationData::getInstance()->servingContexts;
	std::map<std::string, std::vector<WsDetails> > webserviceDetailMap = ConfigurationData::getInstance()->webserviceDetailMap;
	std::map<std::string, bool>::iterator mit;
	for (mit=mycntxts.begin();mit!=mycntxts.end();++mit) {
		if(webserviceDetailMap.find(mit->first)!=webserviceDetailMap.end())
		{
			std::vector<WsDetails> allwsdets = webserviceDetailMap[mit->first];
			wsu.generateAllWSDL(allwsdets, ConfigurationData::getInstance()->coreServerProperties.resourcePath,
					ref, ConfigurationData::getInstance()->classStructureMap);
		}
	}
}
