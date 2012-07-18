/*
	Copyright 2010, Sumeet Chhetri

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

#include "ScriptHandler.h"

Logger ScriptHandler::logger;

ScriptHandler::ScriptHandler() {
	logger = Logger::getLogger("ScriptHandler");
}

ScriptHandler::~ScriptHandler() {
	// TODO Auto-generated destructor stub
}

int ScriptHandler::popenRWE(int *rwepipe, const char *exe, const char *const argv[],string tmpf)
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
		logger << tmpf << endl;
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
		logger << pid << endl;
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

int ScriptHandler::pcloseRWE(int pid, int *rwepipe)
{
	int rc, status;
	close(rwepipe[0]);
	close(rwepipe[1]);
	close(rwepipe[2]);
	rc = waitpid(pid, &status, 0);
	return status;
}

bool ScriptHandler::execute(string command, vector<string> argss, string& output)
{
	bool success = true;
	int pipe[3];
	int pid;
	const char** args = new const char*[argss.size()+2];
	args[0] = command.c_str();
	for (int var = 0; var < (int)argss.size(); ++var) {
		args[var] = argss.at(var).c_str();
	}
	if(argss.size()==0)
	{
		args[1] = NULL;
	}
	else
	{
		args[argss.size()-1] = NULL;
	}
	pid = popenRWEN(pipe, command.c_str(), args);

	char buffer[1024];
	memset(buffer, 0, 1024);
	int length;
	logger << pipe[1] <<" " << pipe[2] << endl;
	while ((length = read(pipe[1], buffer, 1024)) > 0)
	{
		string data(buffer, length);
		output += data;
		logger << data << endl;
		memset(buffer, 0, 1024);
	}
	memset(buffer, 0, 1024);
	if(output=="")
	{
		while ((length = read(pipe[2], buffer, 1024)) > 0)
		{
			string data(buffer, length);
			output += data;
			logger << data << endl;
			memset(buffer, 0, 1024);
			success = false;
		}
		memset(buffer, 0, 1024);
	}
	pcloseRWE(pid, pipe);
	return success;
}

bool ScriptHandler::handle(HttpRequest* req, HttpResponse& res, map<string, string> handoffs, void* dlib,
		string ext, map<string, string> props)
{
	bool skipit = false;
	if(ext==".php")
	{
		skipit = true;
		int pipe[3];
		int pid;
		string def;
		string tmpf = "/temp/";
		string filen;
		if(handoffs.find(req->getCntxt_name())!=handoffs.end())
		{
			def = handoffs[req->getCntxt_name()];
			tmpf = "/";
		}
		string phpcnts = req->toPHPVariablesString(def);
		//logger << phpcnts << endl;
		filen = boost::lexical_cast<string>(Timer::getCurrentTime()) + ".php";
		tmpf = req->getCntxt_root() + tmpf;

		AfcUtil::writeTofile(tmpf+filen, phpcnts, true);
		const char *const args[] = {
				"php",
				filen.c_str(),
				NULL
		};
		pid = popenRWE(pipe, args[0], args, tmpf);

		char buffer[1024];
		memset(buffer, 0, 1024);
		int length;
		string content;
		while ((length = read(pipe[1], buffer, 1024)) != 0)
		{
			//logger << length << endl;
			string data(buffer, length);
			//logger << data << endl;
			content += data;
			memset(buffer, 0, 1024);
		}
		memset(buffer, 0, 1024);
		if(content=="")
		{
			while ((length = read(pipe[2], buffer, 1024)) != 0)
			{
				//logger << length << endl;
				string data(buffer, length);
				//logger << data << endl;
				content += data;
				memset(buffer, 0, 1024);
			}
			memset(buffer, 0, 1024);
		}
		pcloseRWE(pid, pipe);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".html"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".pl")
	{
		skipit = true;
		int pipe[3];
		int pid;

		string phpcnts = req->toPerlVariablesString();
		//logger << phpcnts << endl;
		string tmpf = req->getCntxt_root() + "/temp/" + boost::lexical_cast<string>(Timer::getCurrentTime()) + ".pl";
		//logger << tmpf << endl;
		string plfile = req->getCntxt_root()+"/scripts/perl/"+req->getFile();
		ifstream infile(plfile.c_str());
		string xml;
		if(infile.is_open())
		{
			while(getline(infile, xml))
			{
				phpcnts.append(xml+"\n");
			}
		}
		infile.close();
		AfcUtil::writeTofile(tmpf, phpcnts, true);
		const char *const args[] = {
				"perl",
				tmpf.c_str(),
				NULL
		};
		pid = popenRWE(pipe, args[0], args, "");

		char buffer[1024];
		memset(buffer, 0, 1024);
		int length;
		string content;
		while ((length = read(pipe[1], buffer, 1024)) != 0)
		{
			//logger << length << endl;
			string data(buffer, length);
			//logger << data << endl;
			content += data;
			memset(buffer, 0, 1024);
		}
		memset(buffer, 0, 1024);
		if(content=="")
		{
			while ((length = read(pipe[2], buffer, 1024)) != 0)
			{
				//logger << length << endl;
				string data(buffer, length);
				//logger << data << endl;
				content += data;
				memset(buffer, 0, 1024);
			}
			memset(buffer, 0, 1024);
		}
		pcloseRWE(pid, pipe);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".html"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".rb")
	{
		skipit = true;
		int pipe[3];
		int pid;

		string phpcnts = req->toRubyVariablesString();
		//logger << phpcnts << endl;
		string tmpf = req->getCntxt_root() + "/temp/" + boost::lexical_cast<string>(Timer::getCurrentTime()) + ".rb";
		//logger << tmpf << endl;
		AfcUtil::writeTofile(tmpf, phpcnts, true);
		const char *const args[] = {
				"ruby",
				tmpf.c_str(),
				NULL
		};
		pid = popenRWE(pipe, args[0], args, "");

		char buffer[1024];
		memset(buffer, 0, 1024);
		int length;
		string content;
		while ((length = read(pipe[1], buffer, 1024)) != 0)
		{
			//logger << length << endl;
			string data(buffer, length);
			//logger << data << endl;
			content += data;
			memset(buffer, 0, 1024);
		}
		memset(buffer, 0, 1024);
		if(content=="")
		{
			while ((length = read(pipe[2], buffer, 1024)) != 0)
			{
				//logger << length << endl;
				string data(buffer, length);
				//logger << data << endl;
				content += data;
				memset(buffer, 0, 1024);
			}
			memset(buffer, 0, 1024);
		}

		pcloseRWE(pid, pipe);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".html"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".py")
	{
		skipit = true;
		int pipe[3];
		int pid;

		string phpcnts = req->toPythonVariablesString();
		//logger << phpcnts << endl;
		string tmpf = req->getCntxt_root() + "/temp/" + boost::lexical_cast<string>(Timer::getCurrentTime()) + ".py";
		//logger << tmpf << endl;
		string plfile = req->getCntxt_root()+"/scripts/python/"+req->getFile();
		ifstream infile(plfile.c_str());
		string xml;
		if(infile.is_open())
		{
			while(getline(infile, xml))
			{
				phpcnts.append(xml+"\n");
			}
		}
		infile.close();
		AfcUtil::writeTofile(tmpf, phpcnts, true);
		const char *const args[] = {
				"python",
				tmpf.c_str(),
				NULL
		};
		pid = popenRWE(pipe, args[0], args, "");

		char buffer[1024];
		memset(buffer, 0, 1024);
		int length;
		string content;
		while ((length = read(pipe[1], buffer, 1024)) != 0)
		{
			//logger << length << endl;
			string data(buffer, length);
			//logger << data << endl;
			content += data;
			memset(buffer, 0, 1024);
		}
		memset(buffer, 0, 1024);
		if(content=="")
		{
			while ((length = read(pipe[2], buffer, 1024)) != 0)
			{
				//logger << length << endl;
				string data(buffer, length);
				//logger << data << endl;
				content += data;
				memset(buffer, 0, 1024);
			}
			memset(buffer, 0, 1024);
		}

		pcloseRWE(pid, pipe);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".html"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".lua")
	{
		skipit = true;
		int pipe[3];
		int pid;

		string phpcnts = req->toLuaVariablesString();
		//logger << phpcnts << endl;
		string tmpf = req->getCntxt_root() + "/temp/" + boost::lexical_cast<string>(Timer::getCurrentTime()) + ".lua";
		//logger << tmpf << endl;
		AfcUtil::writeTofile(tmpf, phpcnts, true);
		const char *const args[] = {
				"lua",
				tmpf.c_str(),
				NULL
		};
		pid = popenRWE(pipe, args[0], args, "");

		char buffer[1024];
		memset(buffer, 0, 1024);
		int length;
		string content;
		while ((length = read(pipe[1], buffer, 1024)) != 0)
		{
			//logger << length << endl;
			string data(buffer, length);
			//logger << data << endl;
			content += data;
			memset(buffer, 0, 1024);
		}
		memset(buffer, 0, 1024);
		if(content=="")
		{
			while ((length = read(pipe[2], buffer, 1024)) != 0)
			{
				//logger << length << endl;
				string data(buffer, length);
				//logger << data << endl;
				content += data;
				memset(buffer, 0, 1024);
			}
			memset(buffer, 0, 1024);
		}

		pcloseRWE(pid, pipe);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".html"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	else if(ext==".njs")
	{
		skipit = true;
		int pipe[3];
		int pid;

		string phpcnts = req->toNodejsVariablesString();
		//logger << phpcnts << endl;
		string tmpf = req->getCntxt_root() + "/temp/" + boost::lexical_cast<string>(Timer::getCurrentTime()) + ".njs";
		//logger << tmpf << endl;
		AfcUtil::writeTofile(tmpf, phpcnts, true);
		const char *const args[] = {
				"node",
				tmpf.c_str(),
				NULL
		};
		pid = popenRWE(pipe, args[0], args, "");

		char buffer[1024];
		memset(buffer, 0, 1024);
		int length;
		string content;
		while ((length = read(pipe[1], buffer, 1024)) != 0)
		{
			//logger << length << endl;
			string data(buffer, length);
			//logger << data << endl;
			content += data;
			memset(buffer, 0, 1024);
		}
		memset(buffer, 0, 1024);
		if(content=="")
		{
			while ((length = read(pipe[2], buffer, 1024)) != 0)
			{
				//logger << length << endl;
				string data(buffer, length);
				//logger << data << endl;
				content += data;
				memset(buffer, 0, 1024);
			}
			memset(buffer, 0, 1024);
		}

		pcloseRWE(pid, pipe);
		if((content.length()==0))
		{
			res.setStatusCode("404");
			res.setStatusMsg("Not Found");
			//res.setContent_len("0");
		}
		else
		{
			res.setStatusCode("200");
			res.setStatusMsg("OK");
			res.setContent_type(props[".html"]);
			res.setContent_str(content);
			//res.setContent_len(boost::lexical_cast<string>(content.length()));
		}
	}
	return skipit;
}
