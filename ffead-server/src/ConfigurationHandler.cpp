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


void ConfigurationHandler::listi(string cwd,string type,bool apDir,strVec &folders)
{
	Logger logger = Logger::getLogger("ConfigurationHandler");
	FILE *pipe_fp;
	string command;
	if(chdir(cwd.c_str())!=0)
		return;
	if(type=="/")
		command = ("find . \\( ! -name . -prune \\) \\( -type d -o -type l \\) 2>/dev/null");
	else
		command = ("find . \\( ! -name . -prune \\) \\( -type f -o -type l \\) -name '*"+type+"' 2>/dev/null");
	//command = ("ls -F1 "+cwd+"|grep '"+type+"'");
	logger << ("Searching directory " + cwd + " for pattern " + type) << endl;
	if ((pipe_fp = popen(command.c_str(), "r")) == NULL)
	{
		printf("pipe open error in cmd_list\n");
	}
	int t_char;
	string folderName;
	while ((t_char = fgetc(pipe_fp)) != EOF)
	{
		if(t_char!='\n')
		{
			stringstream ss;
			ss << (char)t_char;
			string temp;
			ss >> temp;
			folderName.append(temp);
		}
		else if(folderName!="")
		{
			StringUtil::replaceFirst(folderName,"*","");
			StringUtil::replaceFirst(folderName,"./","");
			if(folderName.find("~")==string::npos)
			{
				if(apDir)
				{
					if(type=="/")
					{
						folderName = cwd+"/"+folderName+"/";
						StringUtil::replaceFirst(folderName,"//","/");
						folders.push_back(folderName);
					}
					else
					{
						folderName = cwd+"/"+folderName;
						StringUtil::replaceFirst(folderName,"//","/");
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
}

ConfigurationData ConfigurationHandler::handle(strVec webdirs,strVec webdirs1,string incpath,string rtdcfpath,string pubpath,string respath,bool isSSLEnabled,FFEADContext* ffeadContext)
{
	Logger logger = Logger::getLogger("ConfigurationHandler");
	ConfigurationData configurationData;
	configurationData.resourcePath = respath;
	strVec all,afcd,appf,wspath,handoffVec;
	string includeRef;
	StringContext cntxt;
	string libs,ilibs,isrcs,iobjs,ideps;
	Reflection ref;
	vector<bool> stat;
	strVec vecvp,pathvec;
	map<string, string> ajintpthMap, tpes, dcps, compnts;
	propMap srp;
	PropFileReader pread;
	XmlParser parser("Parser");
	ComponentGen gen;
	if(isSSLEnabled)
	{
		propMap sslsec = pread.getProperties(respath+"/security.prop");
		if(sslsec.size()>0)
		{
			configurationData.key_file = sslsec["KEYFILE"];
			configurationData.dh_file = sslsec["DHFILE"];
			configurationData.ca_list = sslsec["CA_LIST"];
			configurationData.rand_file = sslsec["RANDOM"];
			configurationData.sec_password = sslsec["PASSWORD"];
			string tempcl = sslsec["CLIENT_SEC_LEVEL"];
			configurationData.srv_auth_prvd = sslsec["SRV_AUTH_PRVD"];
			configurationData.srv_auth_mode = sslsec["SRV_AUTH_MODE"];
			configurationData.srv_auth_file = sslsec["SRV_AUTH_FILE"];
			if(tempcl!="")
			{
				try
				{
					configurationData.client_auth = CastUtil::lexical_cast<int>(tempcl);
				}
				catch(...)
				{
					logger << "\nInvalid client auth level defined" << flush;
					configurationData.client_auth = 1;
				}
			}
		}
	}
	string rundyncontent;
	for(unsigned int var=0;var<webdirs.size();var++)
	{
		//logger <<  webdirs.at(0) << flush;
		string defpath = webdirs.at(var);
		string dcppath = defpath + "dcp/";
		string tmplpath = defpath + "tpe/";
		string cmppath = defpath + "components/";
		string usrincludes = defpath + "include/";
		//propMap srp = pread.getProperties(defpath+"config/app.prop");

		string name = webdirs1.at(var);
		StringUtil::replaceAll(name,"/","");
		rundyncontent += "cp -Rf $FEAD_CPP_PATH/public/* $FEAD_CPP_PATH/web/"+name+"/public/\n";
		configurationData.cntMap[name] = "true";
		vector<string> adcps;
		listi(dcppath,".dcp",true,adcps);
		for (int var = 0; var < (int)adcps.size(); ++var) {
			dcps[adcps.at(var)] = name;
		}
		vector<string> atpes;
		listi(tmplpath,".tpe",true,atpes);
		for (int var = 0; var < (int)atpes.size(); ++var) {
			tpes[atpes.at(var)] = name;
		}

		vector<string> acompnts;
		listi(cmppath,".cmp",true,acompnts);
		for (int var = 0; var < (int)acompnts.size(); ++var) {
			compnts[acompnts.at(var)] = name;
		}

		all.push_back(usrincludes);
		appf.push_back(defpath+"app.xml");

		libs += ("-l"+ name+" ");
		ilibs += ("-I" + usrincludes+" ");
		wspath.push_back(name);

		logger << "started reading application.xml " << endl;
		Element root = parser.getDocument(defpath+"config/application.xml").getRootElement();
		if(root.getTagName()=="app" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="controllers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="controller")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string clas = cntrls.at(cntn).getAttribute("class");
							string scope = cntrls.at(cntn).getAttribute("scope");
							if(url!="" && clas!="")
							{
								if(cntrls.at(cntn).getAttribute("url").find("*")!=string::npos)
								{
									if(url=="*.*")
										configurationData.urlpattMap[name+url] = clas;
									else
									{
										url = url.substr(url.find("*")+1);
										configurationData.urlMap[name+url] = clas;
									}
								}
								else if(clas!="")
									configurationData.urlMap[name+url] = clas;
								if(clas!="")
								{
									Bean bean("controller_"+name+clas,"",clas,scope,false);
									ffeadContext->addBean(bean);
								}
								logger << ("Adding Controller for " + (name + url) + " :: " + clas) << endl;
							}
							else
							{
								string from = cntrls.at(cntn).getAttribute("from");
								string to = cntrls.at(cntn).getAttribute("to");
								if(to.find("*")!=string::npos && to!="")
									to = to.substr(to.find("*")+1);
								if(from.find("*")!=string::npos && to!="")
								{
									if(from=="*.*")
										configurationData.mappattMap[name+from] = to;
									else
									{
										from = from.substr(from.find("*")+1);
										configurationData.mapMap[name+from] = to;
									}
								}
								else if(to!="")
								{
									configurationData.mapMap[name+from] = to;
								}
								logger << ("Adding Mapping for " + (name + from) + " :: " + to) << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="authhandlers")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="authhandler")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string provider = cntrls.at(cntn).getAttribute("provider");
							string scope = cntrls.at(cntn).getAttribute("scope");
							if(url!="" && provider!="")
							{
								if(url.find("*")!=string::npos)
								{
									if(url=="*.*")
										configurationData.autpattMap[name+url] = provider;
									else
									{
										url = url.substr(url.find("*")+1);
										configurationData.autMap[name+url] = provider;
									}
								}
								else if(provider!="")
									configurationData.autMap[name+url] = provider;
								if(provider!="" && provider.find("class:")!=string::npos)
								{
									string clas = provider.substr(provider.find(":")+1);
									Bean bean("authhandler_"+name+clas,"",clas,scope,false);
									ffeadContext->addBean(bean);
								}
								logger << ("Adding Authhandler for " + (name + url) + " :: " + provider) << endl;
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="filters")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="filter")
						{
							string url = cntrls.at(cntn).getAttribute("url");
							string clas = cntrls.at(cntn).getAttribute("class");
							string type = cntrls.at(cntn).getAttribute("type");
							string scope = cntrls.at(cntn).getAttribute("scope");
							if(clas!="" && (type=="in" || type=="out" || type=="handle"))
							{
								if(url=="")url="*.*";
								if(url.find("*")!=string::npos)
								{
									if(url=="*.*")
									{
										configurationData.filterMap[name+url+type].push_back(clas);
									}
									else
									{
										url = url.substr(url.find("*")+1);
										configurationData.filterMap[name+url+type].push_back(clas);
									}
								}
								if(clas!="")
								{
									Bean bean("filter_"+name+clas,"",clas,scope,false);
									ffeadContext->addBean(bean);
								}
								logger << ("Adding Filter for " + (name + url + type) + " :: " + clas) << endl;
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
							string clas = tmplts.at(tmpn).getAttribute("class");
							configurationData.tmplMap[name+tmplts.at(tmpn).getAttribute("file")] = clas;
							tpes[defpath+tmplts.at(tmpn).getAttribute("file")] = name;
							//tpes.push_back(defpath+tmplts.at(tmpn).getAttribute("file"));
							string scope = tmplts.at(tmpn).getAttribute("scope");
							if(clas!="")
							{
								Bean bean("template_"+name+clas,"",clas,scope,false);
								ffeadContext->addBean(bean);
							}
							logger << ("Adding Template for " + (name+tmplts.at(tmpn).getAttribute("file")) + " :: " + clas) << endl;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="dviews")
				{
					ElementList dvs = eles.at(apps).getChildElements();
					for (unsigned int dn = 0; dn < dvs.size(); dn++)
					{
						if(dvs.at(dn).getTagName()=="dview")
						{
							string clas = dvs.at(dn).getAttribute("class");
							configurationData.vwMap[name+dvs.at(dn).getAttribute("path")] = clas;
							string scope = dvs.at(dn).getAttribute("scope");
							if(clas!="")
							{
								Bean bean("dview_"+name+clas,"",clas,scope,false);
								ffeadContext->addBean(bean);
							}
							logger << ("Adding Dynamic View for " + (name+dvs.at(dn).getAttribute("path")) + " :: " + clas) << endl;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="ajax-interfaces")
				{
					ElementList ajintfs = eles.at(apps).getChildElements();
					for (unsigned int dn = 0; dn < ajintfs.size(); dn++)
					{
						if(ajintfs.at(dn).getTagName()=="ajax-interface")
						{
							string url = ajintfs.at(dn).getAttribute("url");
							if(url.find("*")==string::npos)
							{
								if(url=="")
									url = "/";
								else if(url.at(0)!='/')
									url = "/" + url;
								string clas = ajintfs.at(dn).getAttribute("class");
								configurationData.ajaxIntfMap[name+url] = clas;
								pathvec.push_back(name);
								vecvp.push_back(usrincludes);
								stat.push_back(false);
								ajintpthMap[clas] = "/" + name+url;
								afcd.push_back(clas);
								logger << ("Adding Ajax Interface for " + (name+url) + " :: " + clas) << endl;
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
							string url = cntrls.at(cntn).getAttribute("urlpath");
							string clas = cntrls.at(cntn).getAttribute("class");
							string rname = cntrls.at(cntn).getAttribute("name");
							string scope = cntrls.at(cntn).getAttribute("scope");
							ElementList resfuncs = cntrls.at(cntn).getChildElements();
							for (unsigned int cntn1 = 0; cntn1 < resfuncs.size(); cntn1++)
							{
								if(resfuncs.at(cntn1).getTagName()=="restfunction")
								{
									RestFunction restfunction;
									restfunction.name = resfuncs.at(cntn1).getAttribute("name");
									restfunction.alias = resfuncs.at(cntn1).getAttribute("alias");
									restfunction.clas = clas;
									restfunction.meth = resfuncs.at(cntn1).getAttribute("meth");
									restfunction.baseUrl = resfuncs.at(cntn1).getAttribute("baseUrl");
									if(restfunction.baseUrl!="")
									{
										if(restfunction.baseUrl.at(0)!='/')
											restfunction.baseUrl = "/" + restfunction.baseUrl;
									}
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
											if(param.from=="body")
											{
												hasBodyParam = true;
											}
											if(StringUtil::trimCopy(param.type)=="")
											{
												invalidParam = true;
												logger << "Rest: no type specified for param" << endl;
											}
											else if(param.from!="body" && StringUtil::trimCopy(param.name)=="")
											{
												invalidParam = true;
												logger << "Rest: no name specified for param" << endl;
											}
											else if(param.from=="body" && (restfunction.meth=="GET" || restfunction.meth=="OPTIONS" || restfunction.meth=="TRACE"
													|| restfunction.meth=="HEAD"))
											{
												invalidParam = true;
												logger << ("Rest: skipping param " + param.type + ", from is body and method is " + restfunction.meth) << endl;
											}
											else if(!(param.type=="int" || param.type=="short" || param.type=="long" || param.type=="float" || param.type=="string"
													|| param.type=="std::string" || param.type=="double" || param.type=="bool") && param.from!="body")
											{
												invalidParam = true;
												logger << ("Rest: skipping param " + param.type + ", from is body input is complex type") << endl;
											}
											else if(param.from=="postparam" && (restfunction.meth=="GET" || restfunction.meth=="OPTIONS" || restfunction.meth=="TRACE"
													|| restfunction.meth=="HEAD"))
											{
												invalidParam = true;
												logger << ("Rest: skipping param " + param.type + ", from is postparam and method is " + restfunction.meth) << endl;
											}
											else
												restfunction.params.push_back(param);
										}
									}
									if(hasBodyParam && restfunction.params.size()>1)
									{
										continue;
									}
									if(invalidParam)
									{
										continue;
									}
									if(clas!="")
									{
										if(url.find("*")==string::npos)
										{
											if(url=="")
												url = "/";
											else if(url.at(0)!='/')
												url = "/" + url;
											string urlmpp;
											if(rname!="")
											{
												if(restfunction.alias!="")
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+rname+"/"+restfunction.alias;
													else
														urlmpp = name+restfunction.baseUrl;
													StringUtil::replaceFirst(urlmpp,"//","/");
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
												else
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+rname+"/"+restfunction.name;
													else
														urlmpp = name+restfunction.baseUrl;
													StringUtil::replaceFirst(urlmpp,"//","/");
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
												if(clas!="")
												{
													Bean bean("restcontroller_"+name+clas,"",clas,scope,false);
													ffeadContext->addBean(bean);
												}
											}
											else
											{
												if(restfunction.alias!="")
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+clas+"/"+restfunction.alias;
													else
														urlmpp = name+restfunction.baseUrl;
													StringUtil::replaceFirst(urlmpp,"//","/");
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
												else
												{
													if(restfunction.baseUrl=="")
														urlmpp = name+url+clas+"/"+restfunction.name;
													else
														urlmpp = name+restfunction.baseUrl;
													StringUtil::replaceFirst(urlmpp,"//","/");
													configurationData.rstCntMap[urlmpp] = restfunction;
												}
												if(clas!="")
												{
													Bean bean("restcontroller_"+name+clas,"",clas,scope,false);
													ffeadContext->addBean(bean);
												}
											}
											logger << ("Rest: Adding rest-controller => " + urlmpp  + " , class => " + clas) << endl;
										}
									}
								}
							}
						}
					}
				}
				else if(eles.at(apps).getTagName()=="security")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="login-handler")
						{
							string provider = cntrls.at(cntn).getAttribute("provider");
							string url = cntrls.at(cntn).getAttribute("url");
							string sessionTimeoutV = cntrls.at(cntn).getAttribute("sessionTimeout");
							string scope = cntrls.at(cntn).getAttribute("scope");
							Security securityObject;
							securityObject.loginProvider = provider;
							StringUtil::replaceFirst(url,"//","/");
							if(url.at(0)=='/' && url.length()>1)
								url = url.substr(1);
							securityObject.loginUrl = url;
							try {
								securityObject.sessTimeout = CastUtil::lexical_cast<long>(sessionTimeoutV);
							} catch (...) {
								securityObject.sessTimeout = 3600;
								logger << ("Security: Invalid session timeout value defined, defaulting to 1hour/3600sec") << endl;
							}
							configurationData.securityObjectMap[name] = securityObject;
							if(provider!="" && provider.find("class:")!=string::npos)
							{
								string clas = provider.substr(provider.find(":")+1);
								Bean bean("login-handler_"+name+clas,"",clas,scope,false);
								ffeadContext->addBean(bean);
							}
							logger << ("Security: Adding Login Handler => " + url  + " , provider => " + provider) << endl;
						}
						else if(cntrls.at(cntn).getTagName()=="secure")
						{
							if(configurationData.securityObjectMap.find(name)!=configurationData.securityObjectMap.end())
							{
								Security securityObject = configurationData.securityObjectMap[name];
								string path = cntrls.at(cntn).getAttribute("path");
								string role = cntrls.at(cntn).getAttribute("role");
								SecureAspect secureAspect;
								secureAspect.path = path;
								secureAspect.role = role;
								securityObject.secures.push_back(secureAspect);
								configurationData.securityObjectMap[name] = securityObject;
								logger << ("Security: Adding Secure Path => " + path  + " , role => " + role) << endl;
							}
						}
						else if(cntrls.at(cntn).getTagName()=="welcome")
						{
							string welcomeFile = cntrls.at(cntn).getAttribute("file");
							if(configurationData.securityObjectMap.find(name)!=configurationData.securityObjectMap.end())
							{
								Security securityObject = configurationData.securityObjectMap[name];
								securityObject.welocmeFile = welcomeFile;
								configurationData.securityObjectMap[name] = securityObject;
								logger << ("Security: Adding Welcome file => " + welcomeFile) << endl;
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
							string app = cntrls.at(cntn).getAttribute("app");
							string def = cntrls.at(cntn).getAttribute("default");
							string ext = cntrls.at(cntn).getAttribute("ext");
							configurationData.handoffs[app] = def;
							configurationData.handoffs[app+"extension"] = ext;
							handoffVec.push_back("-l"+ app+" ");
							logger << ("Adding Handoff for app => " + app  + " , ext => " + ext  + " , default url => " + def) << endl;
						}
					}
				}
				else if(eles.at(apps).getTagName()=="cors-config")
				{
					ElementList cntrls = eles.at(apps).getChildElements();
					for (unsigned int cntn = 0; cntn < cntrls.size(); cntn++)
					{
						if(cntrls.at(cntn).getTagName()=="allow-origins")
						{
							configurationData.corsConfig.allwdOrigins = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="allow-methods")
						{
							configurationData.corsConfig.allwdMethods = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="allow-headers")
						{
							configurationData.corsConfig.allwdHeaders = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="allow-credentials")
						{
							try {
								configurationData.corsConfig.allwdCredentials = CastUtil::lexical_cast<bool>(cntrls.at(cntn).getText());
							} catch(...) {}
						}
						else if(cntrls.at(cntn).getTagName()=="expose-headers")
						{
							configurationData.corsConfig.exposedHeaders = cntrls.at(cntn).getText();
						}
						else if(cntrls.at(cntn).getTagName()=="max-age")
						{
							try {
								configurationData.corsConfig.maxAge = CastUtil::lexical_cast<long>(cntrls.at(cntn).getText());
							} catch(...) {}
						}
					}
					logger << ("CORS Configuartion allow-origins => " + configurationData.corsConfig.allwdOrigins
							+ " , allow-methods => " + configurationData.corsConfig.allwdMethods
							+ " , allow-headers => " + configurationData.corsConfig.allwdHeaders
							+ " , allow-credentials => " + CastUtil::lexical_cast<string>(configurationData.corsConfig.allwdCredentials)
							+ " , expose-headers => " + configurationData.corsConfig.exposedHeaders
							+ " , max-age => " + CastUtil::lexical_cast<string>(configurationData.corsConfig.maxAge)) << endl;
				}
			}
		}
		logger << "done reading application.xml " << endl;

		configureCibernate(name, defpath+"config/cibernate.xml");

		logger << "started reading fviews.xml " << endl;
		root = parser.getDocument(defpath+"config/fviews.xml").getRootElement();
		if(root.getTagName()=="fview" && root.getChildElements().size()>0)
		{
			ElementList eles = root.getChildElements();
			for (unsigned int apps = 0; apps < eles.size(); apps++)
			{
				if(eles.at(apps).getTagName()=="page")
				{
					string fvw = eles.at(apps).getAttribute("htm");
					StringUtil::replaceFirst(fvw,".html",".fview");
					configurationData.fviewmap[eles.at(apps).getAttribute("htm")] = eles.at(apps).getAttribute("class");
					pathvec.push_back(name);
					vecvp.push_back(usrincludes);
					stat.push_back(false);
					afcd.push_back(eles.at(apps).getAttribute("class"));
					ElementList elese = eles.at(apps).getChildElements();
					string nsfns = "\nvar _fview_namespace = {";
					string js = "\n\nwindow.onload = function(){";
					for (int appse = 0; appse < (int)elese.size(); appse++)
					{
						if(elese.at(appse).getTagName()=="event")
						{
							nsfns += "\n\"_fview_cntxt_global_js_callback"+CastUtil::lexical_cast<string>(appse)+"\" : function(response){" + elese.at(appse).getAttribute("cb") + "},";
							js += "\ndocument.getElementById('"+elese.at(appse).getAttribute("eid")+"').";
							js += elese.at(appse).getAttribute("type") + " = function(){";
							js += eles.at(apps).getAttribute("class")+"."+elese.at(appse).getAttribute("func")+"(";
							string args = elese.at(appse).getAttribute("args");
							if(args!="")
								args += ",";
							js += args + "\"_fview_cntxt_global_js_callback"+CastUtil::lexical_cast<string>(appse)+"\",\"/"+name+"/"+fvw+"\",_fview_namespace);}";
						}
						else if(elese.at(appse).getTagName()=="form")
						{
							pathvec.push_back(name);
							vecvp.push_back(usrincludes);
							stat.push_back(true);
							afcd.push_back(elese.at(appse).getAttribute("bean"));
							configurationData.formMap[elese.at(appse).getAttribute("name")] = elese.at(appse);
							string clas = elese.at(appse).getAttribute("controller");
							string scope = elese.at(appse).getAttribute("scope");
							if(clas!="")
							{
								Bean bean("form_"+name+clas,"",clas,scope,false);
								ffeadContext->addBean(bean);
							}
							logger << ("Fview: Adding form => " + elese.at(appse).getAttribute("name")
								+ " , form class => " + elese.at(appse).getAttribute("bean")
								+ " , form controller => " + elese.at(appse).getAttribute("controller")) << endl;
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
					AfcUtil::writeTofile(pubpath+eles.at(apps).getAttribute("class")+".js",js,true);

					logger << ("Fview: Adding fview page class => " + eles.at(apps).getAttribute("class")
							+ " , html => " + eles.at(apps).getAttribute("htm")) << endl;
				}
			}
		}
		logger << "done reading fviews.xml " << endl;
	}
	logger << "started generating component code" <<endl;
	map<string, string>::iterator cmpit;
	for (cmpit=compnts.begin();cmpit!=compnts.end();++cmpit)
	{
		string cudata,cuheader,curemote,curemoteheaders;
		try
		{
			string file = gen.generateComponentCU(cmpit->first,cudata,cuheader,curemote,curemoteheaders,cmpit->second);
			AfcUtil::writeTofile(rtdcfpath+file+".h",cuheader,true);
			AfcUtil::writeTofile(rtdcfpath+file+".cpp",cudata,true);
			AfcUtil::writeTofile(rtdcfpath+file+"_Remote.h",curemoteheaders,true);
			AfcUtil::writeTofile(rtdcfpath+file+"_Remote.cpp",curemote,true);
			isrcs += "./"+file+".cpp \\\n"+"./"+file+"_Remote.cpp \\\n";
			iobjs += "./"+file+".o \\\n"+"./"+file+"_Remote.o \\\n";
			ideps += "./"+file+".d \\\n"+"./"+file+"_Remote.d \\\n";
			configurationData.cmpnames.push_back(file);
		}
		catch(const char* ex)
		{
			logger << ("Exception occurred during component code generation : ") << ex << endl;
		}
	}
	for (unsigned int cntn = 0; cntn < handoffVec.size(); cntn++)
	{
		StringUtil::replaceFirst(libs, handoffVec.at(cntn), "");
	}
	logger << "done generating component code" <<endl;
	logger << "started generating reflection/serialization code" <<endl;
	string ret = ref.generateClassDefinitionsAll(all,includeRef,webdirs1);
	string objs, ajaxret, headers,typerefs;
	AfcUtil::writeTofile(rtdcfpath+"ReflectorInterface.cpp",ret,true);
	ret = ref.generateSerDefinitionAll(all,includeRef, true, objs, ajaxret, headers,typerefs,webdirs1);
	AfcUtil::writeTofile(rtdcfpath+"SerializeInterface.cpp",ret,true);
	logger << "done generating reflection/serialization code" <<endl;
	cntxt["RUNTIME_LIBRARIES"] = libs;
	ret = TemplateEngine::evaluate(rtdcfpath+"objects.mk.template",cntxt);
	AfcUtil::writeTofile(rtdcfpath+"objects.mk",ret,true);
	cntxt.clear();
	cntxt["USER_DEFINED_INC"] = ilibs;
	cntxt["RUNTIME_COMP_SRCS"] = isrcs;
	cntxt["RUNTIME_COMP_OBJS"] = iobjs;
	cntxt["RUNTIME_COMP_DEPS"] = ideps;
	ret = TemplateEngine::evaluate(rtdcfpath+"subdir.mk.template",cntxt);
	AfcUtil::writeTofile(rtdcfpath+"subdir.mk",ret,true);
	configurationData.dcpsss = dcps;
	logger << "started generating dcp code" <<endl;
	ret = DCPGenerator::generateDCPAll(dcps);
	AfcUtil::writeTofile(rtdcfpath+"DCPInterface.cpp",ret,true);
	logger << "done generating dcp code" <<endl;
	configurationData.tpes = tpes;
	logger << "started generating template code" <<endl;
	ret = TemplateGenerator::generateTempCdAll(tpes);
	//logger << ret << endl;
	AfcUtil::writeTofile(rtdcfpath+"TemplateInterface.cpp",ret,true);
	logger << "done generating template code" <<endl;
	string infjs;
	logger << endl<< "started generating ajax code" <<endl;
	string ajaxHeaders;
	ret = AfcUtil::generateJsObjectsAll(vecvp,afcd,stat,ajaxHeaders,objs,infjs,pathvec,ajaxret,typerefs,ajintpthMap);
	AfcUtil::writeTofile(rtdcfpath+"AjaxInterface.cpp",ret,true);
	AfcUtil::writeTofile(pubpath+"_afc_Objects.js",objs,true);
	AfcUtil::writeTofile(pubpath+"_afc_Interfaces.js",infjs,true);
	AfcUtil::writeTofile(incpath+"AfcInclude.h",(ajaxHeaders+headers),true);
	logger << "done generating ajax code" <<endl;
	ApplicationUtil apputil;
	webdirs.clear();
	logger << "started generating application code" <<endl;
	ret = apputil.buildAllApplications(appf,webdirs1,configurationData.appMap);
	AfcUtil::writeTofile(rtdcfpath+"ApplicationInterface.cpp",ret,true);
	logger <<  "done generating application code" <<endl;
	WsUtil wsu;
	logger <<  "started generating web-service code" <<endl;
	ret = wsu.generateAllWSDL(wspath,respath,configurationData.wsdlmap);
	AfcUtil::writeTofile(rtdcfpath+"WsInterface.cpp",ret,true);
	logger <<  "done generating web-service code" <<endl;
	cntxt.clear();
	cntxt["TARGET_LIB"] = "all";
	cntxt["Dynamic_Public_Folder_Copy"] = rundyncontent;
	string cont = TemplateEngine::evaluate(respath+"/rundyn_template.sh", cntxt);
	AfcUtil::writeTofile(respath+"/rundyn.sh", cont, true);
	cntxt.clear();
	cntxt["TARGET_LIB"] = "libdinter";
	cntxt["Dynamic_Public_Folder_Copy"] = rundyncontent;
	cont = TemplateEngine::evaluate(respath+"/rundyn_template.sh", cntxt);
	AfcUtil::writeTofile(respath+"/rundyn_dinter.sh", cont, true);

	configurationData.ffeadContext = ffeadContext;

	return configurationData;
}

void ConfigurationHandler::configureCibernate(string name, string configFile)
{
	Logger logger = Logger::getLogger("ConfigurationHandler");
	XmlParser parser("Parser");
	logger << ("started reading cibernate config file " + configFile) << endl;
	Mapping* mapping = new Mapping;
	smstrMap appTableColMapping;
	strMap maptc,maptcl;
	relMap appTableRelMapping;
	Element dbroot = parser.getDocument(configFile).getRootElement();
	if(dbroot.getTagName()=="cibernate")
	{
		ElementList dbeles = dbroot.getChildElements();
		for (unsigned int dbs = 0; dbs < dbeles.size(); dbs++)
		{
			if(dbeles.at(dbs).getTagName()=="config")
			{
				ElementList confs = dbeles.at(dbs).getChildElements();
				string uid,pwd,dsn,dialect;
				int psize= 2;
				for (unsigned int cns = 0; cns < confs.size(); cns++)
				{
					if(confs.at(cns).getTagName()=="uid")
					{
						uid = confs.at(cns).getText();
					}
					else if(confs.at(cns).getTagName()=="pwd")
					{
						pwd = confs.at(cns).getText();
					}
					else if(confs.at(cns).getTagName()=="dsn")
					{
						dsn = confs.at(cns).getText();
					}
					else if(confs.at(cns).getTagName()=="pool-size")
					{
						if(confs.at(cns).getText()!="")
							psize = CastUtil::lexical_cast<int>(confs.at(cns).getText());
					}
					else if(confs.at(cns).getTagName()=="dialect")
					{
						dialect = confs.at(cns).getText();
					}
				}
				CibernateConnPools::addPool(psize,uid,pwd,dsn,name,dialect);
			}
			else if(dbeles.at(dbs).getTagName()=="tables")
			{
				ElementList tabs = dbeles.at(dbs).getChildElements();
				for (unsigned int dn = 0; dn < tabs.size(); dn++)
				{
					if(tabs.at(dn).getTagName()=="table")
					{
						vector<DBRel> relv;
						maptcl[tabs.at(dn).getAttribute("class")] = tabs.at(dn).getAttribute("name");
						ElementList cols = tabs.at(dn).getChildElements();
						for (unsigned int cn = 0; cn < cols.size(); cn++)
						{
							if(cols.at(cn).getTagName()=="hasOne")
							{
								DBRel relation;
								relation.clsName = cols.at(cn).getText();
								relation.type = 1;
								relation.fk = cols.at(cn).getAttribute("fk");
								relation.pk = cols.at(cn).getAttribute("pk");
								relation.field = cols.at(cn).getAttribute("field");
								relv.push_back(relation);
							}
							else if(cols.at(cn).getTagName()=="hasMany")
							{
								DBRel relation;
								relation.clsName = cols.at(cn).getText();
								relation.type = 2;
								relation.fk = cols.at(cn).getAttribute("fk");
								relation.pk = cols.at(cn).getAttribute("pk");
								relation.field = cols.at(cn).getAttribute("field");
								relv.push_back(relation);
							}
							else if(cols.at(cn).getTagName()=="many")
							{
								DBRel relation;
								relation.clsName = cols.at(cn).getText();
								relation.type = 3;
								relation.fk = cols.at(cn).getAttribute("fk");
								relation.pk = cols.at(cn).getAttribute("pk");
								relv.push_back(relation);
							}
							else if(cols.at(cn).getTagName()=="col")
							{
								maptc[cols.at(cn).getAttribute("obf")] = StringUtil::toLowerCopy(cols.at(cn).getAttribute("dbf"));
							}
						}
						appTableColMapping[tabs.at(dn).getAttribute("class")] = maptc;
						appTableRelMapping[tabs.at(dn).getAttribute("class")] = relv;
					}
				}
			}
		}
	}
	mapping->setAppTableColMapping(appTableColMapping);
	mapping->setAppTableClassMapping(maptcl);
	mapping->setAppTableRelMapping(appTableRelMapping);
	CibernateConnPools::addMapping(name,mapping);
	logger << "done reading cibernate config file " + configFile << endl;
}


void ConfigurationHandler::destroyCibernate()
{
	map<string,Mapping*> mappings = CibernateConnPools::getMappings();
	map<string,Mapping*>::iterator it;
	for(it=mappings.begin();it!=mappings.end();it++)
	{
		CibernateConnectionPool* pool = CibernateConnPools::getPool(it->first);
		delete pool;
		delete it->second;
	}
}
