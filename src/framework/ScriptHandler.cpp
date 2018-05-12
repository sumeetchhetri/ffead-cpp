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
 * ScriptHandler.cpp
 *
 *  Created on: Jun 17, 2012
 *      Author: Sumeet
 */
#pragma GCC diagnostic ignored "-Wunused-result"

#include "ScriptHandler.h"

std::string ScriptHandler::chdirExecute(const std::string& exe, const std::string& tmpf, const bool& retErrs)
{
	if(chdir(tmpf.c_str())==0)
		return execute(exe, retErrs);
	return "";
}

std::string ScriptHandler::execute(std::string exe, const bool& retErrs)
{
	std::string data;
	if(retErrs)
	{
		exe += " 2>&1";
	}
	FILE *pp;
	#ifndef OS_MINGW
	pp = popen(exe.c_str(), "r");
	#else
	exe = "sh -c \"" + exe + "\"";
	pp = popen(exe.c_str(), "r");
	#endif
	if (pp != NULL) {
		while (1) {
		  char *line;
		  char buf[1000];
		  memset(buf, 0, sizeof(buf));
		  line = fgets(buf, sizeof buf, pp);
		  if (line == NULL) break;
		  data.append(line);
		  //if (line[0] == 'd') printf("%s", line); /* line includes '\n' */
		}
		pclose(pp);
	}
	return data;
}

#if !defined(OS_MINGW)
int ScriptHandler::popenRWE(int *rwepipe, const char *exe, const char *const argv[], const std::string& tmpf)
{
	int in[2];
	int out[2];
	int err[2];
	int pid;
	int rc;

	rc = pipe(in);
	if (rc<0)
		goto error_in;

	rc = pipe(out);
	if (rc<0)
		goto error_out;

	rc = pipe(err);
	if (rc<0)
		goto error_err;

	pid = fork();
	if (pid > 0) { // parent
		close(in[0]);
		close(out[1]);
		close(err[1]);
		rwepipe[0] = in[1];
		rwepipe[1] = out[0];
		rwepipe[2] = err[0];
		return pid;
	} else if (pid == 0) { // child
		close(in[1]);
		close(out[0]);
		close(err[0]);
		close(0);
		dup(in[0]);
		close(1);
		dup(out[1]);
		close(2);
		dup(err[1]);
		//logger << tmpf << std::endl;
		chdir(tmpf.c_str());
		execvp(exe, (char**)argv);
		exit(1);
	} else
		goto error_fork;

	return pid;

error_fork:
	close(err[0]);
	close(err[1]);
error_err:
	close(out[0]);
	close(out[1]);
error_out:
	close(in[0]);
	close(in[1]);
error_in:
	return -1;
}

int ScriptHandler::popenRWEN(int *rwepipe, const char *exe, const char** argv)
{
	int in[2];
	int out[2];
	int err[2];
	int pid;
	int rc;

	rc = pipe(in);
	if (rc<0)
		goto error_in;

	rc = pipe(out);
	if (rc<0)
		goto error_out;

	rc = pipe(err);
	if (rc<0)
		goto error_err;
	pid = fork();
	if (pid > 0) { // parent
		close(in[0]);
		close(out[1]);
		close(err[1]);
		rwepipe[0] = in[1];
		rwepipe[1] = out[0];
		rwepipe[2] = err[0];
		return pid;
	} else if (pid == 0) { // child
		//logger << pid << std::endl;
		close(in[1]);
		close(out[0]);
		close(err[0]);
		close(0);
		dup(in[0]);
		close(1);
		dup(out[1]);
		close(2);
		dup(err[1]);
		execvp(exe, (char**)argv);
		exit(1);
	} else
		goto error_fork;

	return pid;

error_fork:
	close(err[0]);
	close(err[1]);
error_err:
	close(out[0]);
	close(out[1]);
error_out:
	close(in[0]);
	close(in[1]);
error_in:
	return -1;
}

int ScriptHandler::pcloseRWE(const int& pid, int *rwepipe)
{
	int status;
	close(rwepipe[0]);
	close(rwepipe[1]);
	close(rwepipe[2]);
	waitpid(pid, &status, 0);
	return status;
}
#endif

#ifdef INC_SCRH
bool ScriptHandler::handle(HttpRequest* req, HttpResponse* res, std::map<std::string, std::string>& handoffs, const std::string& ext)
{
	bool skipit = false;
	/*std::string referer = req->getHeader(HttpRequest::Referer);
	if(referer.find("http://")!=std::string::npos)
	{
		std::string appl = referer.substr(referer.find("http://")+7);
		appl = appl.substr(referer.find("/")+1);
		if(appl.find(req->getCntxt_name())==0 && handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			if(appl==req->getCntxt_name()+"/"+handoffs[req->getCntxt_name()])
			{

			}
		}
	}*/
	if(ext==".php")
	{
		skipit = true;
		//int pipe[3];
		//int pid;
		std::string def;
		std::string tmpf = "/temp/";
		std::string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		std::string phpcnts = req->toPHPVariablesString(def);
		//logger << phpcnts << std::endl;
		filen = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime()) + ".php";
		tmpf = req->getContextHome() + tmpf;

		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);

		std::string command = "php " + filen;
		std::string content = chdirExecute(command, tmpf, SCRIPT_EXEC_SHOW_ERRS);
		if((content.length()==0))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res->setContent_len("0");
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(".html"));
			res->setContent(content);
			//res->setContent_len(CastUtil::lexical_cast<std::string>(content.length()));
		}
	}
	else if(ext==".pl")
	{
		skipit = true;
		//int pipe[3];
		//int pid;
		std::string def;
		std::string tmpf = "/temp/";
		std::string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		filen = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime()) + ".pl";
		tmpf = req->getContextHome() + tmpf;
		std::string phpcnts = req->toPerlVariablesString();
		//logger << tmpf << std::endl;
		std::string plfile = req->getUrl();
		std::ifstream infile(plfile.c_str(), std::ios::binary);
		std::string xml;
		if(infile.is_open())
		{
			while(getline(infile, xml))
			{
				phpcnts.append(xml+"\n");
			}
		}
		infile.close();
		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);

		std::string command = "perl " + filen;
		std::string content = chdirExecute(command, tmpf, SCRIPT_EXEC_SHOW_ERRS);
		if((content.length()==0))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res->setContent_len("0");
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(".html"));
			res->setContent(content);
			//res->setContent_len(CastUtil::lexical_cast<std::string>(content.length()));
		}
	}
	else if(ext==".rb")
	{
		skipit = true;
		//int pipe[3];
		//int pid;
		std::string def;
		std::string tmpf = "/temp/";
		std::string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		std::string phpcnts = req->toRubyVariablesString();
		//logger << phpcnts << std::endl;
		filen = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime()) + ".rb";
		tmpf = req->getContextHome() + tmpf;

		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);

		std::string command = "ruby " + filen;
		std::string content = chdirExecute(command, tmpf, SCRIPT_EXEC_SHOW_ERRS);
		if((content.length()==0))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res->setContent_len("0");
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(".html"));
			res->setContent(content);
			//res->setContent_len(CastUtil::lexical_cast<std::string>(content.length()));
		}
	}
	else if(ext==".py")
	{
		skipit = true;
		//int pipe[3];
		//int pid;
		std::string def;
		std::string tmpf = "/temp/";
		std::string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		filen = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime()) + ".py";
		tmpf = req->getContextHome() + tmpf;
		std::string phpcnts = req->toPythonVariablesString();
		std::string plfile = req->getUrl();
		std::ifstream infile(plfile.c_str(), std::ios::binary);
		std::string xml;
		if(infile.is_open())
		{
			while(getline(infile, xml))
			{
				phpcnts.append(xml+"\n");
			}
		}
		infile.close();
		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);

		std::string command = "python " + filen;
		std::string content = chdirExecute(command, tmpf, SCRIPT_EXEC_SHOW_ERRS);
		if((content.length()==0))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res->setContent_len("0");
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(".html"));
			res->setContent(content);
			//res->setContent_len(CastUtil::lexical_cast<std::string>(content.length()));
		}
	}
	else if(ext==".lua")
	{
		skipit = true;
		//int pipe[3];
		//int pid;
		std::string def;
		std::string tmpf = "/temp/";
		std::string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		std::string phpcnts = req->toLuaVariablesString();
		//logger << phpcnts << std::endl;
		filen = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime()) + ".lua";
		tmpf = req->getContextHome() + tmpf;

		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);

		std::string command = "lua " + filen;
		std::string content = chdirExecute(command, tmpf, SCRIPT_EXEC_SHOW_ERRS);
		if((content.length()==0))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res->setContent_len("0");
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(".html"));
			res->setContent(content);
			//res->setContent_len(CastUtil::lexical_cast<std::string>(content.length()));
		}
	}
	else if(ext==".njs")
	{
		skipit = true;
		//int pipe[3];
		//int pid;
		std::string def;
		std::string tmpf = "/temp/";
		std::string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		std::string phpcnts = req->toNodejsVariablesString();
		//logger << phpcnts << std::endl;
		filen = CastUtil::lexical_cast<std::string>(Timer::getCurrentTime()) + ".njs";
		tmpf = req->getContextHome() + tmpf;

		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);

		std::string command = "node " + filen;
		std::string content = chdirExecute(command, tmpf, SCRIPT_EXEC_SHOW_ERRS);
		if((content.length()==0))
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::NotFound);
			//res->setContent_len("0");
		}
		else
		{
			res->setHTTPResponseStatus(HTTPResponseStatus::Ok);
			res->addHeaderValue(HttpResponse::ContentType, CommonUtils::getMimeType(".html"));
			res->setContent(content);
			//res->setContent_len(CastUtil::lexical_cast<std::string>(content.length()));
		}
	}
	return skipit;
}
#endif
