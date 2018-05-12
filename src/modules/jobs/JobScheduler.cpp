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
 * JobScheduler.cpp
 *
 *  Created on: 15-Jul-2013
 *      Author: sumeetc
 */

#include "JobScheduler.h"

JobScheduler* JobScheduler::instance = NULL;

JobScheduler::JobScheduler() {
	isStarted = false;
}

JobScheduler::~JobScheduler() {
}

void JobScheduler::init(const std::string& fileName, const std::string& appName) {
	XmlParser parser("Parser");
	Document doc;
	parser.readDocument(fileName, doc);
	const Element& root = doc.getRootElement();
	if(root.getTagName()=="job-procs" && root.getChildElements().size()>0)
	{
		init(root.getChildElements(), appName);
	}
}

void JobScheduler::init(const ElementList& tabs, const std::string& appName) {
	if(instance!=NULL)
		return;
	Logger logger = LoggerFactory::getLogger("JOB", "JobScheduler");
	instance = new JobScheduler();
	logger << "Initialized JobScheduler" << std::endl;
	for (unsigned int dn = 0; dn < tabs.size(); dn++)
	{
		if(tabs.at(dn).getTagName()!="job-proc")
		{
			logger << "Invalid Element Tag found inside job-procs, should be job-proc..." << std::endl;
			continue;
		}
		std::string clas = tabs.at(dn).getAttribute("class");
		std::string cron = tabs.at(dn).getAttribute("cron");
		std::string meth = tabs.at(dn).getAttribute("method");
		std::string name = tabs.at(dn).getAttribute("name");
		if(clas!="" && meth!="" && cron!="" && name!="")
		{
			JobConfig config;
			config.name = name;
			config.cron = cron;
			config.clas = clas;
			config.meth = meth;
			config.app = appName;
			instance->configs.push_back(config);
			logger << "Added JobConfig inside JobScheduler" << std::endl;
		}
		else
		{
			logger << "Cannot add Job Process as some mandatory elements are missing from the configuration" << std::endl;
		}
	}
}


void JobScheduler::start() {
	Reflector ref;
	if(instance==NULL || (instance!=NULL && instance->isStarted))
		return;
	Logger logger = LoggerFactory::getLogger("JOB", "JobScheduler");
	instance->isStarted = true;
	for (int dn = 0; dn < (int)instance->configs.size(); dn++)
	{
		std::string clas = instance->configs.at(dn).clas;
		std::string cron = instance->configs.at(dn).cron;
		std::string method = instance->configs.at(dn).meth;
		std::string name = instance->configs.at(dn).name;
		std::string appName = instance->configs.at(dn).app;
		if(clas!="" && method!="" && cron!="" && name!="")
		{
			ClassInfo claz = ref.getClassInfo(clas, appName);
			logger << "JobScheduler - Got class " + claz.getClassName() << std::endl;
			if(claz.getClassName()!="")
			{
				args argus;
				Method meth = claz.getMethod(method, argus);
				Constructor ctor = claz.getConstructor(argus);

				logger << "JobScheduler - Got method,class " + meth.getMethodName() + "," + ctor.getName() << std::endl;

				if(meth.getMethodName()!="" && ctor.getName()!="")
				{
					void* objIns = ref.newInstanceGVP(ctor);
					JobFunction f = (JobFunction)ref.getMethodInstance(meth);

					logger << "JobScheduler - Got objins,func " << objIns << "," << f << std::endl;

					if(objIns!=NULL && f!=NULL)
					{
						JobTask* task = new JobTask;
						task->objIns = objIns;
						task->cron = cron;
						task->clas = clas;
						task->name = name;
						task->meth = meth;
						task->appName = appName;
						task->doRun = true;

						Thread* pthread = new Thread(&JobScheduler::service, task);
						pthread->execute();

						instance->tasks.push_back(task);
						logger << "Added Job Process successfully" << std::endl;
					}
					else
					{
						logger << "Cannot add Job Process as could not initialize class instance and method" << std::endl;
					}
				}
				else
				{
					logger << "Cannot add Job Process as method was not found or public no argument constructor not defined for class" << std::endl;
				}
			}
			else
			{
				logger << "Cannot add Job Process as class information was not found" << std::endl;
			}
		}
		else
		{
			logger << "Cannot add Job Process as some mandatory elements are missing from the configuration" << std::endl;
		}
	}
}

void JobScheduler::stop() {
	if(instance==NULL)
	{
		return;
	}
	for (int var = 0; var < (int)instance->tasks.size(); ++var) {
		instance->tasks.at(var)->doRun = false;
	}
	Logger logger = LoggerFactory::getLogger("JOB", "JobScheduler");
	logger << "Waiting 10 seconds for all Job Processes to shutdown....";
	sleep(10);
	for (int var = 0; var < (int)instance->tasks.size(); ++var) {
		//delete instance->tasks.at(var);
	}
	delete instance;
}

void* JobScheduler::service(void* arg)
{
	JobTask *task  = static_cast<JobTask*>(arg);
	task->run();
	delete task;
	return NULL;
}


void JobScheduler::JobTask::run() {
	Logger logger = LoggerFactory::getLogger("JOB", "JobTask");
	try {
		CronTimer timer(cron);
		vals values;
		timer.nextRunDate = Date();

		Reflector ref;
		JobFunction f = (JobFunction)ref.getMethodInstance(meth);

		while(doRun)
		{
			sleep(1);
			Date d2;
			if(timer.isValid(5, d2.getYear(), timer.nextRunDate.getYear()))
			{
				if(timer.isValid(3, d2.getMonth(), timer.nextRunDate.getMonth()))
				{
					if(timer.isValid(2, d2.getDay(), timer.nextRunDate.getDay()))
					{
						if(timer.isValid(1, d2.getHours(), timer.nextRunDate.getHours()))
						{
							if(timer.isValid(0, d2.getMinutes(), timer.nextRunDate.getMinutes()))
							{
								logger << "Running Job Process " + name << std::endl;

								f(objIns, values);

								bool incrementDone = false;

								incrementDone = timer.tryIncrement(0, timer.nextRunDate.getMinutes());
								if(!incrementDone) {
									incrementDone = timer.tryIncrement(1, timer.nextRunDate.getHours());
								}

								if(!incrementDone) {
									incrementDone = timer.tryIncrement(2, timer.nextRunDate.getDay());
								}

								if(!incrementDone) {
									incrementDone = timer.tryIncrement(3, timer.nextRunDate.getMonth());
								}
								if(!incrementDone) {
									incrementDone = timer.tryIncrement(5, timer.nextRunDate.getYear());
								}

								logger << "Running Job Process " + name + " complete" << std::endl;
							}
						}
					}
				}
			}
		}

		if(objIns!=NULL) {
			ref.destroy(objIns, clas, appName);
			objIns = NULL;
		}
	} catch(const char* ex) {
		logger << "Cannot run Job as the cron std::string is invalid" << std::endl;
	}
}

